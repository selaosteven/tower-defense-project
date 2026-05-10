#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <vector>
#include <memory>
#include "Entities/Entity.h"
#include "Entities/Target.h"
#include "Entities/Enemy.h"

class Augment;
class Tower;

class Projectile : public Entity {
public:
    static std::vector<std::shared_ptr<Sprites::Sprite>> createSprites(SDL_Color color = {125,255,200,255});

protected:
    float size_; // Aoe Damage | if 0 then single impact
    float ps_; // Projectile Speed
    float damage_; // Damage of the projectile
    float armor_piercing_; // Armor piercing percentage (0.0 to 1.0)
    float collision_radius_; // Distance of the projectile and the target
    bool has_hit_;
    bool hit_flying_;
    bool hit_ground_;
    std::vector<Augment*> augments_; // Augments list 
    std::unique_ptr<Target> target_; // Target the projectile is moving toard
    std::weak_ptr<Tower> source_tower_;

public:
    Projectile(float size, float ps, float damage = 0.0f, float armor_piercing = 0.0f); 
    Projectile(Point position, float size, float ps, float damage = 0.0f, float armor_piercing = 0.0f); 
    Projectile(const Projectile& other);
    
    /**
     * @brief Update the projectile each frame
     * 
     * @param deltaTime
     */
    void live(float deltaTime) override;
private:
    /**
     * @brief Applies the projectile's damage to all enemies in range
     * 
     * @param enemies
     */
    void do_hit(std::vector<Enemy*> enemies);
    
public: 
    /**
     * @brief Executes the full hit sequence
     * 
     *
     * @param enemies 
     */
    void hit(std::vector<Enemy*> enemies);
    

    inline float getVelocity() const{
        return ps_;
    }
    inline float getSize() const { return size_; }
    inline bool hasHit() const { return has_hit_; }

    inline void setTarget(Enemy * enemy){
        target_ = std::make_unique<TargetEntity>(enemy);
    }
    inline void setTarget(Point position){
        target_ = std::make_unique<TargetPoint>(position);
    }
    inline void setPosition(Point position){ position_ = position; }
    inline void setDamage(float damage) { damage_ = damage; }
    inline void setAugments(std::vector<Augment*> augments) { augments_ = std::move(augments); }
    inline void setSize(float size_incr) {size_+=size_incr;}
    inline void setPs(float ps) { ps_ = ps; }
    inline void setHitFlying(bool val) { hit_flying_ = val; }
    inline void setHitGround(bool val) { hit_ground_ = val; }
    inline void setArmorPiercing(float ap) { armor_piercing_ = ap; }
    inline float getArmorPiercing() const { return armor_piercing_; }
    inline void setSourceTower(std::weak_ptr<Tower> tower) { source_tower_ = tower; }

    std::unique_ptr<Projectile> clone() const;
};

#endif