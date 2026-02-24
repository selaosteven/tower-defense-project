#ifndef TOWER_H
#define TOWER_H

#include <string>
// #include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "Entities/Projectile.h"
#include "Entities/Entity.h"
#include "Sprites/Sprite.h"

class Augment;
class Enemy;

class Tower : public Entity {
// static
protected:
    static int compteur_;

    std::vector<Sprites::Sprite*> createTowerSprites(SDL_Color color = {125,255,200,255});

private:
    // Stats de chaque tours
    float range_; // Range
    float damage_; // Damage
    float as_; // Attack Speed 
    float rs_; // Rotation Speed
    Projectile& proj_; // Classe Projectile 
    std::string type_; // Type de la tour
    int id_; 

protected:
    std::vector<Augment*> augments_;

public:
    Tower(float range, float damage, float as, float rs, Projectile& proj, std::string type); // Constructeur
    Tower(Point position, Tower &t);

// Core methods

private:
    void do_rotate(Enemy& target);
    void do_shoot(Enemy& target);
public:
    void rotate(Enemy& target);
    void shoot(Enemy& target);

// Inline getter
public:
    inline float getRange() const {return range_;} // Getter Range
    inline float getDamage() const {return damage_;} // Getter Damage
    inline float getAs() const {return as_;} // Getter Attack Speed
    inline float getRs() const {return rs_;} // Getter Rotation Speed
    inline std::string getType() const {return type_;} // Getter Type
    inline int getId() const {return id_;} // Getter ID
};

#endif