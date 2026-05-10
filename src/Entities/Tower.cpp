#include <cmath>
#include <algorithm>
#include "Sprites/PrimitiveForm.h"
#include "Entities/Augment.h"
#include "Entities/Tower.h"
#include "Entities/Enemy.h"
#include "Entities/TowerTree.h"

int Tower::compteur_ = 0; // for the ID

Tower::Tower(float range,float damage, float as,  float rs, float armor_piercing, Projectile& proj, std::string type,const std::vector<float>& shapes, int xp,int level) : 
    Entity{{0,0}},
    range_{range}, 
    damage_{damage}, 
    as_{as}, 
    rs_{rs},
    armor_piercing_{armor_piercing},
    cone_angle_{30.0f},
    proj_{proj},
    type_{type},
    id_{compteur_++},
    xp_{xp},
    level_{level},
    current_angle_{0.0f},
    time_since_shot_{0.0f},
    show_range_{false},
    range_changed_{true},
    show_cone_{false},
    cone_changed_{false},
    target_ground_{true},
    target_flying_{false}
    {
        sprites_ = Tower::createSprites(shapes);
        range_sprite_ = Sprites::createColoredCircle(1.0f, {100, 150, 255, 60}, -1.0f);
        cone_sprite_ = Sprites::createCone(1.0f, cone_angle_, {255, 150, 100, 60}, -0.9f);
        cannon_sprite_ = Sprites::rectangle({-0.3f, 0.0f, 8.0f}, 0.55f, 0.125f,{168, 93, 105, 200});
        max_level_sprite_ = Sprites::rectangle({0.0f, 0.0f, 9.0f}, 1.0f, 0.25f, {255, 215, 0, 200}); // Golden bar indicator
    }

void Tower::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    if (cone_changed_) {
        cone_sprite_ = Sprites::createCone(1.0f, cone_angle_, {255, 150, 100, 60}, -0.9f);
        if (cone_sprite_) cone_sprite_->setScale(range_);
        cone_changed_ = false;
    }

    if (range_changed_) {
        if (range_sprite_) range_sprite_->setScale(range_);
        if (cone_sprite_) cone_sprite_->setScale(range_);
        range_changed_ = false;
    }

    if (show_range_ && range_sprite_) {
        Point my_offset = offset + position_ * scale;
        range_sprite_->draw(win, deltaTime, my_offset, scale, rot);
    }

    if (show_cone_ && cone_sprite_) {
        Point my_offset = offset + position_ * scale;
        float radians = current_angle_ * M_PI / 180.0f;
        cone_sprite_->draw(win, deltaTime, my_offset, scale, rot + radians);
    }

    Entity::draw(win, deltaTime, offset, scale, rot);

    if (cannon_sprite_) {
        Point my_offset = offset + position_ * scale;
        float radians = current_angle_ * M_PI / 180.0f;
        cannon_sprite_->draw(win, deltaTime, my_offset, scale, rot + radians);
    }

    // Draw the max level indicator on top if applicable
    if (level_ >= levelMax_ && max_level_sprite_) {
        Point my_offset = offset + position_ * scale;
        max_level_sprite_->draw(win, deltaTime, my_offset, scale, rot);
    }

}

void Tower::rotate(std::shared_ptr<Enemy> target){
    for(auto& a : augments_){
        a->tower_rotate_prefix(*this, target);
    }

    do_rotate(target);

    for(auto& a : augments_){
        a->tower_rotate_postfix(*this, target);
    }
}


void Tower::shoot(std::shared_ptr<Enemy> target){
    for(auto& a : augments_){
        a->tower_shoot_prefix(*this, target);
    }

    do_shoot(target);

    for(auto& a : augments_){
        a->tower_shoot_postfix(*this, target, proj_);
    }
}

void Tower::addAugment(std::shared_ptr<Augment> augment) {
    augment->onEquip(*this);
    augments_.push_back(std::move(augment));
}

void Tower::removeAugment(const std::string& name) {
    auto it = std::find_if(augments_.begin(), augments_.end(),
        [&name](const std::shared_ptr<Augment>& aug) {
            return aug->getName() == name;
        });
    if (it != augments_.end()) {
        (*it)->onUnequip(*this);
        
        // Move the augment to detached_augments_ instead of deleting it immediately.
        detached_augments_.push_back(std::move(*it));
        augments_.erase(it);
    }
}

void Tower::clearDetachedAugments() {
    detached_augments_.clear();
}

void Tower::applyUpgrade(const UpgradeNode* node) {
    if (!node) return;
    currentUpgradeNode_ = node;
    
    for (const auto& augName : node->augments) {
        auto augment = TowerTree::createAugment(augName);
        if (augment) {
            addAugment(std::move(augment));
        }
    }
}

void Tower::resetToRootUpgrade() {
    if (currentUpgradeNode_ && currentUpgradeNode_->parent) {
        // Traverse up to find the root node
        const UpgradeNode* root = currentUpgradeNode_;
        while (root->parent) {
            root = root->parent;
        }
        currentUpgradeNode_ = root;
    }
}

void Tower::do_shoot(std::shared_ptr<Enemy> target) {
    auto proj = proj_.clone();
    proj->setPosition(position_);
    proj->setTarget(target);
    proj->setDamage(damage_);
    proj->setArmorPiercing(armor_piercing_);
    
    std::vector<std::shared_ptr<Augment>> proj_augs;
    for(auto& a : augments_) {
        proj_augs.push_back(a);
    }
    proj->setAugments(std::move(proj_augs));
    proj->setHitFlying(target_flying_);
    proj->setHitGround(target_ground_);
    spawned_projectiles_.push_back(std::move(proj));
}

void Tower::do_rotate(std::shared_ptr<Enemy> target) {
}

float Tower::normalizeAngle(float angle) const {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

float Tower::getSmallestRotation(float from, float to) const {
    float diff = normalizeAngle(to - from);
    return diff;
}

float Tower::calculateAngleToTarget(const std::shared_ptr<Enemy> target) const {
    Point direction = target->getPosition() ^ position_;
    float angle = std::atan2(direction.getY(), direction.getX()) * 180.0f / M_PI;
    return normalizeAngle(angle);
}

bool Tower::isInCone(const std::shared_ptr<Enemy> target) const {
    float angle_to_target = calculateAngleToTarget(target);
    float angle_diff = std::abs(normalizeAngle(angle_to_target - current_angle_));
    
    // If angle difference is greater than 180, take the smaller angle
    if (angle_diff > 180.0f) angle_diff = 360.0f - angle_diff;
    
    return angle_diff <= (cone_angle_ / 2.0f);
}

std::shared_ptr<Enemy> Tower::findBestTarget(const std::vector<std::shared_ptr<Enemy>>& enemies) {
    std::shared_ptr<Enemy> best_target = nullptr;
    float smallest_rotation = 360.0f; // Maximum rotation needed

    for (auto enemy : enemies) {
        if (!enemy || !enemy->isAlive()) continue;
        
        // Filter out enemies this tower is not allowed to hit
        if (enemy->isFlying() && !target_flying_) continue;
        if (!enemy->isFlying() && !target_ground_) continue;
        
        // Check distance
        Point direction = enemy->getPosition() ^ position_;
        float distance = std::sqrt(direction.getX() * direction.getX() + direction.getY() * direction.getY());
        
        if (distance > range_) continue;
        
        // Calculate rotation needed to face this target
        float target_angle = calculateAngleToTarget(enemy);
        float rotation_needed = std::abs(getSmallestRotation(current_angle_, target_angle));
        
        // Pick the target that needs the least rotation
        if (rotation_needed < smallest_rotation) {
            smallest_rotation = rotation_needed;
            best_target = enemy;
        }
    }

    return best_target;
}

void Tower::live(float deltaTime, const std::vector<std::shared_ptr<Enemy>>& enemies) {
    time_since_shot_ += deltaTime;
    
    // Find best target 
    std::shared_ptr<Enemy> target = findBestTarget(enemies);
    
    if (!target) {
        return; 
    }
    
    // Rotate towards target
    float target_angle = calculateAngleToTarget(target);
    float rotation_needed = getSmallestRotation(current_angle_, target_angle);
    float max_rotation = rs_ * deltaTime;
    
    if (std::abs(rotation_needed) > max_rotation) {
        // Still need to rotate
        current_angle_ += (rotation_needed > 0 ? max_rotation : -max_rotation);
        current_angle_ = normalizeAngle(current_angle_);
    } else {
        // Hit target angle
        current_angle_ = target_angle;
    }
    
    // Check if target is in cone and can shoot
    if (isInCone(target) && time_since_shot_ >= (1.0f / as_)) {
        shoot(target);
        time_since_shot_ = 0.0f;
    }

    // Level Up Setup
    while (level_ < levelMax_ && xp_ >= xpMax_) {  
        xp_ -= xpMax_;
        level_++;

        damage_ *= 1.05f;
        range_  += 0.1f;
        as_     += 0.02f;
        rs_     += 1.0f;

        // At each level threshold, we increase a stat
        if(level_ == 4){
            setAs(as_*1.1);
        }
        if(level_ == 7){
            setRs(1.25*rs_);
        }
        if(level_ == 11){
            setRange(range_*1.25);
        }

        std::cout << "Tower " << id_ << " leveled up to " << level_ << "!\n";
    }
    
    if (level_ >= levelMax_) { // Max Level Reach
        xp_ = xpMax_; 
    }
}

std::vector<std::shared_ptr<Sprites::Sprite>> Tower::createSprites(const std::vector<float>& shapes) {
    std::vector<std::shared_ptr<Sprites::Sprite>> out;

    size_t i = 0;
    while (i < shapes.size()) {

        int type = static_cast<int>(shapes[i++]);

        std::array<float,3> pos = {
            0.0f,
            0.0f,
            0.0f
        };

        if (type == 0) { // Rectangle
            float w = shapes[i++];
            float h = shapes[i++];
            SDL_Color col = { 
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]),
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]) 
            };
            out.push_back(Sprites::rectangle(pos, w, h, col));
        }

        else if (type == 1) { // Circle
            float r = shapes[i++];
            SDL_Color col = { 
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]),
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]) 
            };

            auto c = Sprites::createColoredCircle(r, col, 0.0f);
            out.push_back(c);
        }

        else if (type == 2) { // Triangle
            float size = shapes[i++];
            float orientation = shapes[i++];
            SDL_Color col = { 
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]),
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]) 
            };

            Sprites::Orientation ori = static_cast<Sprites::Orientation>(orientation);
            out.push_back(
                Sprites::triangle(pos, size, col, ori)
            );
        }

        else if (type == 3) { // Octagon
            float size = shapes[i++];
            SDL_Color col = { 
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]),
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]) 
            };

            out.push_back(Sprites::octone(pos, size, col));
        }

        else if (type == 4) { // Square
            float side = shapes[i++];
            SDL_Color col = { 
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]),
                static_cast<Uint8>(shapes[i++]), 
                static_cast<Uint8>(shapes[i++]) 
            };

            out.push_back(Sprites::rectangle(pos, side, side, col));
        }
    }

    return out;
}
