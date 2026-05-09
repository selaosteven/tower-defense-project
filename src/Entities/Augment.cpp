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

// CritAugment

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

// RangeAugment

RangeAugment::RangeAugment(float bonusRange) : Augment("Range Boost"), bonusRange_(bonusRange) {}

void RangeAugment::onEquip(Tower& tower) {
    tower.setRange(tower.getRange() + bonusRange_);
}

void RangeAugment::onUnequip(Tower& tower) {
    tower.setRange(tower.getRange() - bonusRange_);
}

// TargetingAugment

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

// SlownessEffect to properly track and reset enemy speed
class SlownessEffect : public Effect {
    float slowAmount_;
public:
    SlownessEffect(float duration, float amount) 
        : Effect("Slowness", duration), slowAmount_(amount) {}

    void onStart(Enemy& target) override {
        target.setSpeed(target.getSpeed() * (1.0f - slowAmount_));
    }

    void onEnd(Enemy& target) override {
        target.setSpeed(target.getSpeed() / (1.0f - slowAmount_));
    }

    void resetTimer() {
        timer_ = 0.0f;
    }
};

// SlownessAugment

SlownessAugment::SlownessAugment(float amount)
    : Augment("Slowness"), slowAmount_(amount) {}

void SlownessAugment::projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& p) {
    for (auto* e : enemies) {
        if (!e || !e->isAlive()) continue;

        bool already_slowed = false;
        for (const auto& effect : e->getEffects()) {
            if (effect->getName() == "Slowness") {
                already_slowed = true;
                static_cast<SlownessEffect*>(effect.get())->resetTimer();
                break;
            }
        }

        if (!already_slowed) {
            e->addEffect(std::make_unique<SlownessEffect>(2.0f, slowAmount_));
        }
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

void AoeAugment::onEquip(Tower& tower) {
    tower.getBaseProjectile().setSize(size_incr_);
}

void AoeAugment::onUnequip(Tower& tower) {
    tower.getBaseProjectile().setSize(-size_incr_);
}

// RotationSpeedAugment

RotationSpeedAugment::RotationSpeedAugment(float amount) : Augment("Rotation Speed"), amount_(amount) {}
void RotationSpeedAugment::onEquip(Tower& tower) {
    tower.setRs(tower.getRs() + amount_);
}

// AttackSpeedAugment

AttackSpeedAugment::AttackSpeedAugment(float multiplier) : Augment("Attack Speed"), multiplier_(multiplier) {}
void AttackSpeedAugment::onEquip(Tower& tower) {
    tower.setAs(tower.getAs() * multiplier_);
}

// ProjectileSpeedAugment

ProjectileSpeedAugment::ProjectileSpeedAugment(float multiplier) : Augment("Projectile Speed"), multiplier_(multiplier) {}
void ProjectileSpeedAugment::onEquip(Tower& tower) {
    tower.getBaseProjectile().setPs(tower.getBaseProjectile().getVelocity() * multiplier_);
}

// SplashRadiusAugment

SplashRadiusAugment::SplashRadiusAugment(float amount) : Augment("Splash Radius"), amount_(amount) {}
void SplashRadiusAugment::onEquip(Tower& tower) {
    float baseSize = tower.getBaseProjectile().getSize();
    if(baseSize <= 0) baseSize = 0.01f;
    tower.getBaseProjectile().setSize(amount_*baseSize);
}

// ArmorPiercingAugment

ArmorPiercingAugment::ArmorPiercingAugment(float amount) : Augment("Armor Piercing"), amount_(amount) {}
void ArmorPiercingAugment::onEquip(Tower& tower) {
    tower.setArmorPiercing(tower.getArmorPiercing() + amount_);
}

void ArmorPiercingAugment::onUnequip(Tower& tower) {
    tower.setArmorPiercing(tower.getArmorPiercing() - amount_);
}