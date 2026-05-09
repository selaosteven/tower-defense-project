#include "Entities/Augment.h"
#include "Entities/Tower.h"
#include <cstdlib>
#include <iostream>
#include <unordered_set>

Augment::Augment(const std::string& name) : name_{name} {}

Augment::~Augment() {}

// Default empty implementations
void Augment::onEquip(Tower& tower) {}
void Augment::onUnequip(Tower& tower) {}

void Augment::projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& projectile) {}
void Augment::projectile_hit_postfix(std::vector<Enemy*> enemies, Projectile& projectile) {}

void Augment::tower_shoot_prefix(Tower& tower, Enemy& target) {}
void Augment::tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p) {}

void Augment::tower_rotate_prefix(Tower& tower, Enemy& target) {}
void Augment::tower_rotate_postfix(Tower& tower, Enemy& target) {}

void Augment::effect_apply_prefix(Effect& effect, Enemy& target) {}
void Augment::effect_apply_postfix(Effect& effect, Enemy& target) {}

// --- CritAugment Implementation ---

CritAugment::CritAugment(float critChance, float critMultiplier) 
    : Augment("Critical Hit"), critChance_(critChance), critMultiplier_(critMultiplier), originalDamage_(0.0f), didCrit_(false) {}

void CritAugment::tower_shoot_prefix(Tower& tower, Enemy& target) {
    float roll = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    if (roll <= critChance_) {
        didCrit_ = true;
        originalDamage_ = tower.getDamage();
        tower.setDamage(originalDamage_ * critMultiplier_);
        std::cout << "CRITICAL HIT!" << std::endl;
    } else {
        didCrit_ = false;
    }
}

void CritAugment::tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p) {
    if (didCrit_) {
        tower.setDamage(originalDamage_);
    }
}

// --- RangeAugment Implementation ---

RangeAugment::RangeAugment(float bonusRange) : Augment("Range Boost"), bonusRange_(bonusRange) {}

void RangeAugment::onEquip(Tower& tower) {
    tower.setRange(tower.getRange() + bonusRange_);
}

void RangeAugment::onUnequip(Tower& tower) {
    tower.setRange(tower.getRange() - bonusRange_);
}

// --- TargetingAugment Implementation ---

TargetingAugment::TargetingAugment(bool targetGround, bool targetFlying) 
    : Augment("Targeting Modification"), targetGround_(targetGround), targetFlying_(targetFlying) {}

void TargetingAugment::onEquip(Tower& tower) {
    tower.setTargetGround(targetGround_);
    tower.setTargetFlying(targetFlying_);
}

void TargetingAugment::onUnequip(Tower& tower) {
    tower.setTargetGround(true);
    tower.setTargetFlying(false);
}

// SlownessAugment

SlownessAugment::SlownessAugment(float amount)
    : Augment("Slowness"), slowAmount_(amount) {}

void SlownessAugment::projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& p) {
    // for (auto* e : enemies) {
    //     if (!e || !e->isAlive()) continue;

        
    //     // Appliquer un slow directement à l’ennemi
    //     e->setSpeed(e->getSpeed() * (1.0f - slowAmount_));
    //     // Tu peux ajouter un vrai SlowEffect plus tard
    // }

    static std::unordered_set<Enemy*> slowed;

    for (auto* e : enemies) {
        if (!e || !e->isAlive()) continue;

        // Si déjà ralenti → on ne le ralentit plus
        if (slowed.find(e) != slowed.end())
            continue;

        // Appliquer le slow UNE SEULE FOIS
        e->setSpeed(e->getSpeed() * (1.0f - slowAmount_));

        // Marquer comme ralenti
        slowed.insert(e);
    }
}



// DamageAugment

DamageAugment::DamageAugment() : Augment("Damage+") {}

void DamageAugment::onEquip(Tower &tower){
    tower.setDamage(tower.getDamage()*2);
}

void DamageAugment::onUnequip(Tower &tower){
    tower.setDamage(tower.getDamage()/2);
}

// AoeAugment

AoeAugment::AoeAugment(float size) : Augment("Zone Buff"), size_incr_(size) {}

void AoeAugment::tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p) {
    p.setSize(p.getSize() + size_incr_);
}


RotationSpeedAugment::RotationSpeedAugment(float amount) : Augment("Rotation Speed"), amount_(amount) {}
void RotationSpeedAugment::onEquip(Tower& tower) {
    tower.setRs(tower.getRs() + amount_);
}

AttackSpeedAugment::AttackSpeedAugment(float multiplier) : Augment("Attack Speed"), multiplier_(multiplier) {}
void AttackSpeedAugment::onEquip(Tower& tower) {
    // Multiplies Attack Speed
    tower.setAs(tower.getAs() * multiplier_);
}

ProjectileSpeedAugment::ProjectileSpeedAugment(float multiplier) : Augment("Projectile Speed"), multiplier_(multiplier) {}
void ProjectileSpeedAugment::onEquip(Tower& tower) {
    // Updates the core template projectile that the tower clones
    tower.getBaseProjectile().setPs(tower.getBaseProjectile().getVelocity() * multiplier_);
}

SplashRadiusAugment::SplashRadiusAugment(float amount) : Augment("Splash Radius"), amount_(amount) {}
void SplashRadiusAugment::onEquip(Tower& tower) {
    // Increases the projectile's area of effect size! 
    // (This works seamlessly since Session.cpp checks `if (proj->getSize() > 0.0f)` for splash collisions!)
    tower.getBaseProjectile().setSize(amount_);
}