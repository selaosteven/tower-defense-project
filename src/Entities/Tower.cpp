#include "Entities/Augment.h"
#include "Entities/Tower.h"
#include "Entities/Enemy.h"

int Tower::compteur_ = 0;

// Constructeur
Tower::Tower(float range,float damage, float as,  float rs, Projectile proj, std::string type) : 
    range_{range}, 
    damage_{damage}, 
    as_{as}, 
    rs_{rs},
    proj_{proj},
    type_{type},
    id_{compteur_++} 
    {} 


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