#ifndef TOWER_H
#define TOWER_H

class Tower{
protected:
    // Stats de chaque tours
    double range; // Range
    double damage; // Damage
    double as; // Attack Speed 
    double rs; // Rotation Speed
    double ps; // Projectile Speed
    int id; 
    static int compteur;

public:
    Tower(double range, double damage, double as, double rs, double ps); // Constructeur
    int getId() const; // Getter ID

};

#endif