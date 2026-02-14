#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include <vector>

class Entity{
protected:
    std::array<float,2> position; // Vecteur de position
    float orientation; // Angle degré
    Animation anim; // Animation d'une entité
public:
    void Animation();
};


#endif