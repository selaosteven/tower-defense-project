#ifndef TOWER_H
#define TOWER_H

#include "Projectile.h"
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


class Tower {
private:
    // Stats de chaque tours
    float range_; // Range
    float damage_; // Damage
    float as_; // Attack Speed 
    float rs_; // Rotation Speed
    Projectile proj_; // Classe Projectile 
    std::string type_; // Type de la tour
    int id_; 
    static int compteur_;

public:
    Tower(float range, float damage, float as, float rs, Projectile proj, std::string type); // Constructeur
    inline float getRange() const {return range_;} // Getter Range
    inline float getDamage() const {return damage_;} // Getter Damage
    inline float getAs() const {return as_;} // Getter Attack Speed
    inline float getRs() const {return rs_;} // Getter Rotation Speed
    inline std::string getType() const {return type_;} // Getter Type
    inline int getId() const {return id_;} // Getter ID
};

#endif