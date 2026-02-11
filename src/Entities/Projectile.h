#ifndef PROJECTILE_H
#define PROJECTILE_H

class Projectile {
protected:
    double size; // Dégat de Zone ou fixe
    double ps; // Projectile Speed
public:
    Projectile(double size, double ps); // Constructeur
};

#endif