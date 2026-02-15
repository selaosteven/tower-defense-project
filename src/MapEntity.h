// Acces Spatial
#ifndef MAPENTITY_H
#define MAPENTITY_H

#include <string>
#include <vector>
#include <iostream>
#include <array>
#include "Entities/Entity.h"

class MapEntity {
private:
    std::vector<Entity> lst_; // Liste des entités
    std::vector<std::string> map_; // la Map Physique
public:
    MapEntity(std::vector<Entity> lst,std::vector<std::string> map);
    std::vector<Entity> allWithinRange(Point center,float range);
};

#endif