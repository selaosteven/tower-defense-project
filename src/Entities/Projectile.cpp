#include <vector>
#include <cmath>
#include "Entities/Projectile.h"
#include "Entities/Enemy.h"
#include "Entities/Augment.h"
#include "Entities/Target.h"
#include "Sprites/PrimitiveForm.h"

Projectile::Projectile(Point position, float size, float ps, float damage, float armor_piercing) 
    : Entity{position}, size_{size}, ps_{ps}, damage_{damage}, armor_piercing_{armor_piercing}, collision_radius_{0.5f}, has_hit_{false}, hit_flying_{false}, hit_ground_{true}, augments_{} {
    sprites_ = Projectile::createSprites({25,25,25,255});
}

Projectile::Projectile(float size, float ps, float damage, float armor_piercing) 
    : Projectile{{0,0}, size, ps, damage, armor_piercing} {}

Projectile::Projectile(const Projectile& other) 
    : Entity{other.position_}, size_{other.size_}, ps_{other.ps_}, damage_{other.damage_}, armor_piercing_{other.armor_piercing_}, collision_radius_{other.collision_radius_}, has_hit_{false}, hit_flying_{false}, hit_ground_{true}, augments_{other.augments_} {
    sprites_ = Projectile::createSprites({25,25,25,255}); 
}

std::unique_ptr<Projectile> Projectile::clone() const {
    return std::make_unique<Projectile>(*this);
}

void Projectile::do_hit(std::vector<Enemy*> enemies){
    for (auto* enemy : enemies) {
        if (enemy && enemy->isAlive() && (enemy->isFlying() == hit_flying_ || !enemy->isFlying() == hit_ground_)) {
            float effective_resistance = std::max(0.0f, enemy->getResistance() * (1.0f - armor_piercing_));
            float actual_damage = std::max(0.0f, damage_ - effective_resistance);
            enemy->takeDamage(actual_damage);
        }
    }
}

void Projectile::hit(std::vector<Enemy*> enemies){
    for(auto* a : augments_) {
        a->projectile_hit_prefix(enemies, *this);
    }
    do_hit(enemies);

    for(auto* a : augments_) {
        a->projectile_hit_postfix(enemies, *this);
    }
}

void Projectile::live(float deltaTime) {
    if (!target_) return; 
    
    Point targetPos = target_->getPosition();
    Point direction = position_ ^ targetPos;
    float dist = std::sqrt(direction.getX()*direction.getX() + direction.getY()*direction.getY());
    
    if (dist <= collision_radius_) {
        has_hit_ = true;
        return;
    }
    
    float step = std::min(1.0f, (getVelocity() * deltaTime) / dist);
    
    Point velocity = direction * step;
    position_ += velocity;
}

std::vector<std::shared_ptr<Sprites::Sprite>> Projectile::createSprites(SDL_Color color) {
    auto Core = Sprites::circle({0.0f,0.0f,1.0f}, 0.05f);
    return {Core};
}