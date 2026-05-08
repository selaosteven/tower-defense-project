#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <vector>
#include <memory>
#include "Entities/Entity.h"
#include "Entities/Target.h"
#include "Entities/Enemy.h"

class Augment;

class Projectile : public Entity {
// Static
public:
    static std::vector<std::shared_ptr<Sprites::Sprite>> createSprites(SDL_Color color = {125,255,200,255});


protected:
    float size_; // Dégat de Zone ou fixe
    float ps_; // Projectile Speed
    float damage_;
    float collision_radius_;
    bool has_hit_;
    std::vector<Augment*> augments_;
    std::unique_ptr<Target> target_;

public:
    Projectile(float size, float ps, float damage = 0.0f); // Constructeur
    Projectile(Point position, float size, float ps, float damage = 0.0f); // Constructeur
    Projectile(const Projectile& other);
    
    void live(float deltaTime) override;
// Methods
private:
    void do_hit(std::vector<Enemy*> enemies);
public: 
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

    std::unique_ptr<Projectile> clone() const;
};

#endif