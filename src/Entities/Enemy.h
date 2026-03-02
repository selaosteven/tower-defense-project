#ifndef ENEMY_H
#define ENEMY_H

#include "Entities/Entity.h"

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
public:
    std::list<Point>::iterator path_;
    float offset_;
   
public:
    Enemy(float lp, float speed, float resistance, bool fly); // constructeur
    Enemy(Point position, float offset, const Enemy& ref, std::list<Point>::iterator start);

    void live(float deltaTime) override;

};

#endif