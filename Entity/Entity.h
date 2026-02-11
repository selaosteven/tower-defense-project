#ifndef ENTITY_H
#define ENTITY_H


class Entity{
protected:
    std::vector<float> position; // Vecteur de position
    float orientation; // Angle degré
    Animation anim; // Animation d'une entité
public:
    void Animation();
};


#endif