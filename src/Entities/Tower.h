#ifndef TOWER_H
#define TOWER_H

#include <string>
// #include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

#include "Entities/Projectile.h"
#include "Entities/Entity.h"
#include "Sprites/Sprite.h"
#include "Entities/Augment.h"

class Enemy;
struct UpgradeNode;

class Tower : public Entity {
// static
protected:
    static int compteur_;

    std::vector<std::shared_ptr<Sprites::Sprite>> createSprites(const std::vector<float>& shapes);

private:
    // Stats de chaque tours
    float range_; // Range
    float damage_; // Damage
    float as_; // Attack Speed 
    float rs_; // Rotation Speed
    float cone_angle_; // Cone of fire angle in degrees (e.g., 60)
    Projectile proj_; // Classe Projectile 
    std::string type_; // Type de la tour
    int id_;
    int xp_;
    int level_;
    int xpMax_ = 100;
    int levelMax_ = 20;
    
    // Targeting and orientation
    float current_angle_; // Current tower orientation (-180 to 180)
    float time_since_shot_; // Time since last shot 
    bool show_range_;
    bool range_changed_;
    std::shared_ptr<Sprites::Sprite> range_sprite_;
    bool show_cone_;
    bool cone_changed_;
    std::shared_ptr<Sprites::Sprite> cone_sprite_;
    std::shared_ptr<Sprites::Sprite> cannon_sprite_;
    std::shared_ptr<Sprites::Sprite> max_level_sprite_;
    bool target_ground_;
    bool target_flying_;
    std::vector<std::unique_ptr<Projectile>> spawned_projectiles_;

protected:
    std::vector<std::unique_ptr<Augment>> augments_;
    const UpgradeNode* currentUpgradeNode_ = nullptr;

public:
    // Prevent the compiler from implicitly copying the tower and its unique_ptrs
    Tower(const Tower&) = delete;
    Tower& operator=(const Tower&) = delete;

    Tower(float range, float damage, float as, float rs, Projectile& proj, std::string type,const std::vector<float>& shapes, int xp, int level); // Constructeur

// Core methods

private:
    void do_rotate(Enemy& target);
    void do_shoot(Enemy& target);
    
    // Cone targeting helpers
    Enemy* findBestTarget(const std::vector<Enemy*>& enemies);
    float calculateAngleToTarget(const Enemy& target) const;
    float normalizeAngle(float angle) const;
    float getSmallestRotation(float from, float to) const;
    bool isInCone(const Enemy& target) const;
public:
    void rotate(Enemy& target);
    void shoot(Enemy& target);
    void live(float deltaTime, const std::vector<Enemy*>& enemies);
    void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) override;

    void addAugment(std::unique_ptr<Augment> augment);
    void removeAugment(const std::string& name);
    

    inline void setPosition(Point p) { position_ = p; }

    // Tree architecture
    void applyUpgrade(const UpgradeNode* node);
    const UpgradeNode* getCurrentUpgradeNode() const { return currentUpgradeNode_; }

// Inline getter
public:
    inline float getRange() const {return range_;} // Getter Range
    inline float getDamage() const {return damage_;} // Getter Damage
    inline float getAs() const {return as_;} // Getter Attack Speed
    inline float getRs() const {return rs_;} // Getter Rotation Speed
    inline std::string getType() const {return type_;} // Getter Type
    inline int getId() const {return id_;} // Getter ID
    inline Projectile& getBaseProjectile() { return proj_; }

    inline void setRange(float range) { range_ = range; range_changed_ = true; }
    inline void setDamage(float damage) { damage_ = damage; }
    inline void setAs(float as) { as_ = as; }
    inline void setRs(float rs) { rs_ = rs; }
    inline void setConeAngle(float angle) { cone_angle_ = angle; cone_changed_ = true; }
    inline float getConeAngle() const { return cone_angle_; }
    inline float getCurrentAngle() const { return current_angle_; }
    inline void setShowRange(bool show) { show_range_ = show; }
    inline bool getShowRange() const { return show_range_; }
    inline void setShowCone(bool show) { show_cone_ = show; }
    inline bool getShowCone() const { return show_cone_; }
    
    inline void setTargetGround(bool val) { target_ground_ = val; }
    inline bool getTargetGround() const { return target_ground_; }
    inline void setTargetFlying(bool val) { target_flying_ = val; }
    inline bool getTargetFlying() const { return target_flying_; }

    inline void setCannonSprite(std::shared_ptr<Sprites::Sprite> sprite) { cannon_sprite_ = sprite; }
    inline void removeCannonSprite() { cannon_sprite_.reset(); }
    inline Sprites::Sprite* getCannonSprite() const { return cannon_sprite_.get(); }
    inline bool hasCannon() const { return cannon_sprite_ != nullptr; }

    inline int getXp() const {return xp_;}
    inline int getXpMax() const {return xpMax_;}
    inline int getLevel() const {return level_;}
    inline int getLevelMax() const {return levelMax_;}

    std::vector<std::unique_ptr<Projectile>> fetchSpawnedProjectiles() {
        return std::move(spawned_projectiles_);
    }
};

#endif