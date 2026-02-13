#include "Tower.h"

int Tower::compteur_ = 0;

// Constructeur
Tower::Tower(float range,float damage, float as,  float rs, Projectile proj, std::string type) : 
    range_{range}, 
    damage_{damage}, 
    as_{as}, 
    rs_{rs},
    proj_{proj},
    type_{type},
    id_{compteur_++} 
    {} 
