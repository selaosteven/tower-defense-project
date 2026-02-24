#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <vector>

class Enemy;
class Augment;

class Projectile {
protected:
    double size_; // Dégat de Zone ou fixe
    double ps_; // Projectile Speed
    std::vector<Augment*>* augments_;
public:
    Projectile(double size, double ps); // Constructeur

// Methods
private:
    void do_hit(std::vector<Enemy*> enemies);
public: 
    void hit(std::vector<Enemy*> enemies);
};

#endif