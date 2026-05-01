#ifndef AUGMENT_H
#define AUGMENT_H
#include <vector>
#include <string>

#include "Entities/Effect.h"

class Projectile;
class Enemy;
class Tower;

class Augment
{
protected:
    std::string name_;

public:
    Augment(const std::string& name = "Unknown Augment");
    virtual ~Augment();
    
    const std::string& getName() const { return name_; }

    // Methods 
public:
    virtual void onEquip(Tower& tower);
    virtual void onUnequip(Tower& tower);

    virtual void projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& projectile);
    virtual void projectile_hit_postfix(std::vector<Enemy*> enemies, Projectile& projectile);

    // Before the tower shoot, it has a target but no projectile.
    // After the shot there is now a projectil created. 
    // ? keep or change this behaviour ? 
    virtual void tower_shoot_prefix(Tower& tower, Enemy& target);
    virtual void tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p);

    virtual void tower_rotate_prefix(Tower& tower, Enemy& target);
    virtual void tower_rotate_postfix(Tower& tower, Enemy& target);

    virtual void effect_apply_prefix(Effect& effect, Enemy& target);
    virtual void effect_apply_postfix(Effect& effect, Enemy& target);
};

// Example of a Critical Hit Augment
class CritAugment : public Augment {
private:
    float critChance_;
    float critMultiplier_;
    float originalDamage_;
    bool didCrit_;

public:
    CritAugment(float critChance, float critMultiplier);

    void tower_shoot_prefix(Tower& tower, Enemy& target) override;
    void tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p) override;
};

// Example of a Stat Boost Augment (Base block modifier)
class RangeAugment : public Augment {
private:
    float bonusRange_;
public:
    RangeAugment(float bonusRange);
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

#endif
