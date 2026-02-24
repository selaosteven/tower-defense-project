#include "Sprites/PrimitiveForm.h"
#include "Entities/Augment.h"
#include "Entities/Tower.h"
#include "Entities/Enemy.h"

int Tower::compteur_ = 0;

// Constructeur
Tower::Tower(float range,float damage, float as,  float rs, Projectile& proj, std::string type) : 
    Entity{{0,0}},
    range_{range}, 
    damage_{damage}, 
    as_{as}, 
    rs_{rs},
    proj_{proj},
    type_{type},
    id_{compteur_++} 
    {
        sprites_ = Tower::createTowerSprites();
    }

Tower::Tower(Point position, Tower &t) : Entity{position},
range_{t.range_}, 
damage_{t.damage_}, 
as_{t.as_}, 
rs_{t.rs_},
proj_{t.proj_},
type_{t.type_},
id_{compteur_++} 
  {
    sprites_ = Tower::createTowerSprites();
  }


void Tower::rotate(Enemy& target){
    for(auto a : augments_){
        a->tower_rotate_prefix(*this, target);
    }

    do_rotate(target);

    for(auto a : augments_){
        a->tower_rotate_postfix(*this, target);
    }
}


void Tower::shoot(Enemy& target){
    for(auto a : augments_){
        a->tower_shoot_prefix(*this, target);
    }

    do_shoot(target);

    for(auto a : augments_){
        a->tower_shoot_prefix(*this, target);
    }
}


void Tower::do_shoot(Enemy& target) {
    std::cout << "piou piou" << std::endl;
}
void Tower::do_rotate(Enemy& target) {
    std::cout << "rotating rotating" << std::endl;
}


// Static methods 

std::vector<Sprites::Sprite*> Tower::createTowerSprites(SDL_Color color) {
    // base : 
    Sprites::PrimitiveForm * base = Sprites::rectangle({0.0f,0.0f,1.0f}, 20);
    Sprites::PrimitiveForm * socle = Sprites::circle({0.0f,0.0f,2.0f}, 10, 30);
    Sprites::PrimitiveForm * canon = Sprites::rectangle({0.0f,0.0f,3.0f}, 5, 15);
    return {base, socle, canon};
}