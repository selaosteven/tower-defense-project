#ifndef ENEMY_H
#define ENEMY_H

#include "Entities/Entity.h"

class Enemy : public Entity {
protected:
    // Stats de chaque ennemies
    float lp_; // Life Point
    float speed_; // Speed
    float resistance_; // Resistance
    bool fly_; // Fly
   
public:
    Enemy(float lp, float speed, float resistance, bool fly); // constructeur
};

#endif