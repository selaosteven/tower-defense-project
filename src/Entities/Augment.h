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

public:
    /**
     * @brief applies the augment effect of the tower
     * 
     * @param tower 
     */
    virtual void onEquip(Tower& tower);

    /**
     * @brief remove the augment effect of the tower
     * 
     * @param tower 
     */
    virtual void onUnequip(Tower& tower);

    /**
     * @brief called before the creation of the projectile and apply effects
     * 
     * @param enemies 
     * @param projectile 
     */
    virtual void projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& projectile);

    /**
     * @brief called after the creation of the projectile and apply effects
     * 
     * @param enemies 
     * @param projectile 
     */
    virtual void projectile_hit_postfix(std::vector<Enemy*> enemies, Projectile& projectile);

    /**
     * @brief called before the tower fires at its target
     * 
     * @param tower 
     * @param target 
     */
    virtual void tower_shoot_prefix(Tower& tower, Enemy& target);

    /**
     * @brief called after the tower fires and the projectile is created
     * 
     * @param tower 
     * @param target 
     * @param p
     */
    virtual void tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p);

    /**
     * @brief  called before the tower rotates toward its target
     * 
     * @param tower 
     * @param target 
     */
    
    virtual void tower_rotate_prefix(Tower& tower, Enemy& target);
    
    /**
     * @brief called after the tower rotates toward its target
     * 
     * @param effect 
     * @param target 
     */
    virtual void tower_rotate_postfix(Tower& tower, Enemy& target);

    /**
     * @brief called before an effect is applied to an enemy
     * 
     * @param effect 
     * @param target 
     */
    virtual void effect_apply_prefix(Effect& effect, Enemy& target);
    
    /**
     * @brief called after an effect is applied to an enemy
     * 
     * @param effect
     * @param target
     */
    virtual void effect_apply_postfix(Effect& effect, Enemy& target);
    
};

// CritAugment Class
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

// RangeAugment Class
class RangeAugment : public Augment {
private:
    float bonusRange_;
public:
    RangeAugment(float bonusRange);
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

// TargetingAugment Class
class TargetingAugment : public Augment {
private:
    bool targetGround_;
    bool targetFlying_;
public:
    TargetingAugment(bool targetGround, bool targetFlying);
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

// SlownessAugment Class
class SlownessAugment : public Augment {
    private:
        float slowAmount_;

    public:
        SlownessAugment(float amount);
        void projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& p) override;
};

// DamageAugment Class
class DamageAugment : public Augment {
    public:
        DamageAugment();
        void onEquip(Tower& tower) override;
        void onUnequip(Tower& tower) override;
};

// AoeAugment Class
class AoeAugment : public Augment {
    private:
        float size_incr_;

    public:
        AoeAugment(float size);
        void onEquip(Tower& tower) override;
        void onUnequip(Tower& tower) override;
};

// RotationSpeedAugment Class
class RotationSpeedAugment : public Augment {
    float amount_;
public:
    RotationSpeedAugment(float amount);
    void onEquip(Tower& tower) override;
};

// AttackSpeedAugment Class
class AttackSpeedAugment : public Augment {
    float multiplier_;
public:
    AttackSpeedAugment(float multiplier);
    void onEquip(Tower& tower) override;
};

// ProjectileSpeedAugment Class
class ProjectileSpeedAugment : public Augment {
    float multiplier_;
public:
    ProjectileSpeedAugment(float multiplier);
    void onEquip(Tower& tower) override;
};

// SplashRadiusAugment Class
class SplashRadiusAugment : public Augment {
    float amount_;
public:
    SplashRadiusAugment(float amount);
    void onEquip(Tower& tower) override;
};

// ArmorPiercingAugment Class
class ArmorPiercingAugment : public Augment {
    float amount_;
public:
    ArmorPiercingAugment(float amount);
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

// CloseCombatAugment Class (Trade-off: +Damage, +Rotation Speed, -Range)
class CloseCombatAugment : public Augment {
public:
    CloseCombatAugment();
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

// SniperScopeAugment Class (Trade-off: +Range, +Armor Piercing, -Attack Speed, -Rotation Speed)
class SniperScopeAugment : public Augment {
public:
    SniperScopeAugment();
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

// OverclockedGearAugment Class (Trade-off: ++Attack Speed, -Damage, -Range)
class OverclockedGearAugment : public Augment {
public:
    OverclockedGearAugment();
    void onEquip(Tower& tower) override;
    void onUnequip(Tower& tower) override;
};

#endif
