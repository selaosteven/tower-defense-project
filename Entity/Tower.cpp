#include "Tower.h"
#include <iostream>

int Tower::compteur = 0;

// Constructeur
Tower::Tower(double range, double damage, double as, double rs, double ps) : 
range(range), damage(damage), as(as), rs(rs), ps(ps) ,id(compteur++) {} 

int Tower::getId() const {
    return id;
}
