#ifndef ENEMY_H
#define ENEMY_H

#include "Entities/Entity.h"
#include "Entities/Effect.h"
#include <memory>
#include <string>

class Enemy : public Entity {

    // Static
public:
    static std::vector<std::shared_ptr<Sprites::Sprite>> createSprites(bool is_flying, const std::string& display_char, SDL_Color color, float size);

protected:
    // Stats de chaque ennemies
    float lp_; // Life Point
    float speed_; // Speed
    float resistance_; // Resistance
    bool fly_; // Fly
    std::string display_char_;
    std::vector<std::unique_ptr<Effect>> effects_; // The Effect "stack"
public:
    std::list<Point>::iterator path_;
    std::list<Point>::iterator path_end_;
    float offset_;
    bool reached_end_ = false;
    bool alive_ = true;
   
public:
    Enemy(float lp, float speed, float resistance, bool fly); // constructeur
    Enemy(float lp, float speed, float resistance, bool fly, std::string display_char);
    Enemy(float lp, float speed, float resistance, bool fly, std::string display_char, SDL_Color color);
    Enemy(float lp, float speed, float resistance, bool fly, std::string display_char, SDL_Color color, float size);
    Enemy(Point position, float offset, const Enemy& ref, std::list<Point>::iterator start, std::list<Point>::iterator end);
    virtual ~Enemy() = default;

    void live(float deltaTime) override;
    bool hasReachedEnd() const { return reached_end_; }
    bool isAlive() const { return alive_; }
    void kill() { alive_ = false; }
    bool isFlying() const { return fly_; }

    // Methods to manage effects
    void addEffect(std::unique_ptr<Effect> effect);
    const std::vector<std::unique_ptr<Effect>>& getEffects() const { return effects_; }

    // Accessors for effects to modify stats
    float getSpeed() const { return speed_; }
    void setSpeed(float speed) { speed_ = speed; }
    float getLp() const { return lp_; }
    void takeDamage(float amount) { lp_ -= amount; }

    // Setters for instantiation
    void setPosition(Point p) { position_ = p; }
    void setPath(std::list<Point>::iterator start, std::list<Point>::iterator end) { path_ = start; path_end_ = end; }
    void setOffset(float offset) { offset_ = offset; }

};

#endif