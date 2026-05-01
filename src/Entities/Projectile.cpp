#include <vector>
#include "Entities/Projectile.h"
#include "Entities/Enemy.h"
#include "Entities/Augment.h"
#include "Entities/Target.h"
#include "Sprites/PrimitiveForm.h"


void Projectile::do_hit(std::vector<Enemy*> enemies){

}

void Projectile::hit(std::vector<Enemy*> enemies){
    if (augments_) {
        for(auto& a : *augments_) {
            a->projectile_hit_prefix(enemies, *this);
        }
    }
    do_hit(enemies);
    if (augments_) {
        for(auto& a : *augments_) {
            a->projectile_hit_postfix(enemies, *this);
        }
    }
}

Projectile::Projectile(Point position, float size, float ps) : Entity{position}, size_{size}, ps_{ps}, augments_{nullptr} {
    sprites_ = Projectile::createSprites({25,25,25,255});
}
Projectile::Projectile(float size, float ps) : Projectile{{0,0}, size, ps} {}


void Projectile::live(float deltaTime) {
    if (!target_) return; // Sécurité si la cible n'est pas encore définie
    Point direction = position_ ^ target_->getPosition();
    Point velocity = (direction | (getVelocity() * deltaTime));
    position_ += velocity;
}


// Static methods


std::vector<Sprites::Sprite*> Projectile::createSprites(SDL_Color color) {
    // base : 
    Sprites::PrimitiveForm * Core = Sprites::circle({0.0f,0.0f,1.0f}, 5);
    Sprites::PrimitiveForm * Behind = Sprites::rectangle({0.0f,-5.0f,1.0f}, 5);

    return {Core, Behind};
}