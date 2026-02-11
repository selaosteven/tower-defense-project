#ifndef TOWER_H
#define TOWER_H

#include <Projectile.h>
#include <string.h>

class Tower{
protected:
    // Stats de chaque tours
    double range; // Range
    double damage; // Damage
    double as; // Attack Speed 
    double rs; // Rotation Speed
    Projectile proj; // Classe Projectile 
    std::string type; // Type de la tour
    int id; 
    static int compteur;

public:
    Tower(double range, double damage, double as, double rs, Projectile proj, std::string type); // Constructeur
    int getId() const; // Getter ID

};

#endif