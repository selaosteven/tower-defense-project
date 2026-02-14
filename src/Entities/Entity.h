#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include <vector>
#include "Point.h"

class Entity{
protected:
    Point position_; // Vecteur de position
    float orientation_; // Angle degré
    // Animation anim; // Animation d'une entité
public:
    Entity(Point position, float orientation);

    inline Point getPosition() const {return position_;}
    inline float getOrientation() const {return orientation_;}

    // void Animation();
};


#endif