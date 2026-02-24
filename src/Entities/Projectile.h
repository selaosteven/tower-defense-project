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
    std::vector<Sprites::Sprite*> createSprites(SDL_Color color = {125,255,200,255});


protected:
    float size_; // Dégat de Zone ou fixe
    float ps_; // Projectile Speed
    std::vector<Augment*>* augments_;
    std::unique_ptr<Target> target_;

public:
    Projectile(float size, float ps); // Constructeur
    Projectile(Point position, float size, float ps); // Constructeur
    
    void live(float deltaTime) override;
// Methods
private:
    void do_hit(std::vector<Enemy*> enemies);
public: 
    void hit(std::vector<Enemy*> enemies);

    inline float getVelocity() const{
        return ps_;
    }

    inline void setTarget(Enemy * enemy){
        target_ = std::make_unique<TargetEntity>(enemy);
    }
    inline void setTarget(Point position){
        target_ = std::make_unique<TargetPoint>(position);
    }
};

#endif