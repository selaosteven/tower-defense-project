#include "Tower.h"

int Tower::compteur = 0;

// Constructeur
Tower::Tower(double range, double damage, double as, double rs, Projectile proj, std::string type) : 
range(range), damage(damage), as(as), rs(rs),type(type),id(compteur++) {} 

int Tower::getId() const {
    return id;
}
