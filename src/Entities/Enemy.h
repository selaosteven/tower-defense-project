#ifndef ENEMY_H
#define ENEMY_H

#include "Entities/Entity.h"
#include "Entities/Effect.h"
#include <memory>

class Enemy : public Entity {

    // Static
public:
    static std::vector<Sprites::Sprite*> createSprites(SDL_Color color = {125,255,200,255});


protected:
    // Stats de chaque ennemies
    float lp_; // Life Point
    float speed_; // Speed
    float resistance_; // Resistance
    bool fly_; // Fly
    std::vector<std::unique_ptr<Effect>> effects_; // The Effect "stack"
public:
    std::list<Point>::iterator path_;
    float offset_;
   
public:
    Enemy(float lp, float speed, float resistance, bool fly); // constructeur
    Enemy(Point position, float offset, const Enemy& ref, std::list<Point>::iterator start);

    void live(float deltaTime) override;

    // Methods to manage effects
    void addEffect(std::unique_ptr<Effect> effect);
    const std::vector<std::unique_ptr<Effect>>& getEffects() const { return effects_; }

    // Accessors for effects to modify stats
    float getSpeed() const { return speed_; }
    void setSpeed(float speed) { speed_ = speed; }
    float getLp() const { return lp_; }
    void takeDamage(float amount) { lp_ -= amount; }

};

#endif