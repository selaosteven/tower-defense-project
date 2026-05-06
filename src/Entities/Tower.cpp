#include "Sprites/PrimitiveForm.h"
#include "Entities/Augment.h"
#include "Entities/Tower.h"
#include "Entities/Enemy.h"
#include "Entities/TowerTree.h"

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
        sprites_ = Tower::createSprites();
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
    sprites_ = Tower::createSprites();
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
    std::cout << "piou piou" << std::endl;
}
void Tower::do_rotate(Enemy& target) {
    std::cout << "rotating rotating" << std::endl;
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