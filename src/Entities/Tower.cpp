#include <cmath>
#include "Sprites/PrimitiveForm.h"
#include "Entities/Augment.h"
#include "Entities/Tower.h"
#include "Entities/Enemy.h"
#include "Entities/TowerTree.h"

namespace {
    Sprites::PrimitiveForm* createColoredCircle(float radius, SDL_Color color, float zindex) {
        std::vector<SDL_Vertex> vertices;
        const float pi = std::acos(-1.0f);
        const int points = 60; // Higher point count for smooth big circles
        const float bangle = 2.0f * pi / points;
        SDL_Vertex center{{0.0f, 0.0f}, color, {0.0f, 0.0f}};
        for (int i = 0; i < points; i++) {
            vertices.push_back(center);
            vertices.push_back({{static_cast<float>(std::cos(i * -bangle)) * radius, static_cast<float>(std::sin(i * -bangle)) * radius}, color, {0.0f, 0.0f}});
            vertices.push_back({{static_cast<float>(std::cos((i + 1) * -bangle)) * radius, static_cast<float>(std::sin((i + 1) * -bangle)) * radius}, color, {0.0f, 0.0f}});
        }
        return new Sprites::PrimitiveForm({0.0f, 0.0f, zindex}, std::move(vertices));
    }
}

int Tower::compteur_ = 0;

Tower::Tower(float range,float damage, float as,  float rs, Projectile& proj, std::string type) : 
    Entity{{0,0}},
    range_{range}, 
    damage_{damage}, 
    as_{as}, 
    rs_{rs},
    cone_angle_{60.0f},
    proj_{proj},
    type_{type},
    id_{compteur_++},
    current_angle_{0.0f},
    time_since_shot_{0.0f},
    show_range_{false},
    range_changed_{true},
    target_ground_{true},
    target_flying_{false} // By default, towers only target ground enemies!
    {
        sprites_ = Tower::createSprites();
        range_sprite_.reset(createColoredCircle(1.0f, {100, 150, 255, 60}, -1.0f));
    }

Tower::Tower(Point position, Tower &t) : Entity{position},
range_{t.range_}, 
damage_{t.damage_}, 
as_{t.as_}, 
rs_{t.rs_},
cone_angle_{t.cone_angle_},
proj_{t.proj_},
type_{t.type_},
id_{compteur_++},
current_angle_{0.0f},
time_since_shot_{0.0f},
show_range_{false},
range_changed_{true},
target_ground_{t.target_ground_},
target_flying_{t.target_flying_}
  {
    sprites_ = Tower::createSprites();
    range_sprite_.reset(createColoredCircle(1.0f, {100, 150, 255, 60}, -1.0f));
  }

void Tower::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    if (show_range_ && range_sprite_) {
        if (range_changed_) {
            range_sprite_->setScale(range_);
            range_changed_ = false;
        }
        Point my_offset = offset + position_ * scale;
        range_sprite_->draw(win, deltaTime, my_offset, scale, rot);
    }
    Entity::draw(win, deltaTime, offset, scale, rot);
}

void Tower::rotate(Enemy& target){
    for(auto& a : augments_){
        a->tower_rotate_prefix(*this, target);
    }

    do_rotate(target);

    for(auto& a : augments_){
        a->tower_rotate_postfix(*this, target);
    }
}


void Tower::shoot(Enemy& target){
    for(auto& a : augments_){
        a->tower_shoot_prefix(*this, target);
    }

    do_shoot(target);

    for(auto& a : augments_){
        a->tower_shoot_postfix(*this, target, proj_);
    }
}

void Tower::addAugment(std::unique_ptr<Augment> augment) {
    augment->onEquip(*this);
    augments_.push_back(std::move(augment));
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


void Tower::do_shoot(Enemy& target) {
    auto proj = proj_.clone();
    proj->setPosition(position_);
    proj->setTarget(&target);
    proj->setDamage(damage_);
    
    std::vector<Augment*> proj_augs;
    for(auto& a : augments_) {
        proj_augs.push_back(a.get());
    }
    proj->setAugments(std::move(proj_augs));
    
    spawned_projectiles_.push_back(std::move(proj));
}
void Tower::do_rotate(Enemy& target) {
}

// Cone targeting helpers

float Tower::normalizeAngle(float angle) const {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

float Tower::getSmallestRotation(float from, float to) const {
    float diff = normalizeAngle(to - from);
    return diff;
}

float Tower::calculateAngleToTarget(const Enemy& target) const {
    Point direction = target.getPosition() ^ position_;
    float angle = std::atan2(direction.getY(), direction.getX()) * 180.0f / M_PI;
    return normalizeAngle(angle);
}

bool Tower::isInCone(const Enemy& target) const {
    float angle_to_target = calculateAngleToTarget(target);
    float angle_diff = std::abs(normalizeAngle(angle_to_target - current_angle_));
    
    // If angle difference is greater than 180, take the smaller angle
    if (angle_diff > 180.0f) angle_diff = 360.0f - angle_diff;
    
    return angle_diff <= (cone_angle_ / 2.0f);
}

Enemy* Tower::findBestTarget(const std::vector<Enemy*>& enemies) {
    Enemy* best_target = nullptr;
    float smallest_rotation = 360.0f; // Maximum rotation needed

    for (auto* enemy : enemies) {
        if (!enemy || !enemy->isAlive()) continue;
        
        // Filter out enemies this tower is not allowed to hit
        if (enemy->isFlying() && !target_flying_) continue;
        if (!enemy->isFlying() && !target_ground_) continue;
        
        // Check distance
        Point direction = enemy->getPosition() ^ position_;
        float distance = std::sqrt(direction.getX() * direction.getX() + direction.getY() * direction.getY());
        
        if (distance > range_) continue;
        
        // Calculate rotation needed to face this target
        float target_angle = calculateAngleToTarget(*enemy);
        float rotation_needed = std::abs(getSmallestRotation(current_angle_, target_angle));
        
        // Pick the target that needs the least rotation
        if (rotation_needed < smallest_rotation) {
            smallest_rotation = rotation_needed;
            best_target = enemy;
        }
    }

    return best_target;
}

void Tower::live(float deltaTime, const std::vector<Enemy*>& enemies) {
    time_since_shot_ += deltaTime;
    
    // Find best target (target needing least rotation within range)
    Enemy* target = findBestTarget(enemies);
    
    if (!target) {
        return; // No valid target
    }
    
    // Rotate towards target
    float target_angle = calculateAngleToTarget(*target);
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
    if (isInCone(*target) && time_since_shot_ >= (1.0f / as_)) {
        shoot(*target);
        time_since_shot_ = 0.0f;
    }
}

// Static methods 

std::vector<Sprites::Sprite*> Tower::createSprites(SDL_Color color) {
    // Sizes are now in logical units, relative to a 1x1 cell, to match the map sprites.
    // A tower should fit comfortably within a single cell.
    Sprites::PrimitiveForm * base = Sprites::rectangle({0.0f, 0.0f, 1.0f}, 0.9f, 0.9f); // A square base almost filling the cell
    Sprites::PrimitiveForm * socle = Sprites::circle({0.0f, 0.0f, 2.0f}, 0.4f, 30);      // A circular platform on top of the base
    Sprites::PrimitiveForm * canon = Sprites::rectangle({0.0f, 0.0f, 3.0f}, 0.15f, 0.5f); // The cannon itself

    return {base, socle, canon};
}