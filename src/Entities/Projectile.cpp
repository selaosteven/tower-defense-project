#include <vector>
#include <cmath>
#include "Entities/Projectile.h"
#include "Entities/Enemy.h"
#include "Entities/Augment.h"
#include "Entities/Target.h"
#include "Sprites/PrimitiveForm.h"

Projectile::Projectile(Point position, float size, float ps, float damage) 
    : Entity{position}, size_{size}, ps_{ps}, damage_{damage}, collision_radius_{0.5f}, has_hit_{false}, augments_{} {
    sprites_ = Projectile::createSprites({25,25,25,255});
}

Projectile::Projectile(float size, float ps, float damage) 
    : Projectile{{0,0}, size, ps, damage} {}

Projectile::Projectile(const Projectile& other) 
    : Entity{other.position_}, size_{other.size_}, ps_{other.ps_}, damage_{other.damage_}, collision_radius_{other.collision_radius_}, has_hit_{false}, augments_{other.augments_} {
    sprites_ = Projectile::createSprites({25,25,25,255}); 
}

std::unique_ptr<Projectile> Projectile::clone() const {
    return std::make_unique<Projectile>(*this);
}

void Projectile::do_hit(std::vector<Enemy*> enemies){
    for (auto* enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->takeDamage(damage_);
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
    if (!target_) return; // Sécurité si la cible n'est pas encore définie
    
    Point targetPos = target_->getPosition();
    Point direction = position_ ^ targetPos;
    float dist = std::sqrt(direction.getX()*direction.getX() + direction.getY()*direction.getY());
    
    if (dist <= collision_radius_) {
        has_hit_ = true;
        return;
    }
    
    Point velocity = (direction | (getVelocity() * deltaTime));
    position_ += velocity;
}

std::vector<Sprites::Sprite*> Projectile::createSprites(SDL_Color color) {
    Sprites::PrimitiveForm * Core = Sprites::circle({0.0f,0.0f,1.0f}, 0.15f);
    return {Core};
}