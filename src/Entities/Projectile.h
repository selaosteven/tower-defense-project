#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <vector>
#include "Entities/Entity.h"

class Enemy;
class Augment;

class Projectile : public Entity {
// Static
public:
    std::vector<Sprites::Sprite*> createSprites(SDL_Color color = {125,255,200,255});


protected:
    double size_; // Dégat de Zone ou fixe
    double ps_; // Projectile Speed
    std::vector<Augment*>* augments_;
public:
    Projectile(double size, double ps); // Constructeur
    Projectile(Point position, double size, double ps); // Constructeur

// Methods
private:
    void do_hit(std::vector<Enemy*> enemies);
public: 
    void hit(std::vector<Enemy*> enemies);
};

#endif