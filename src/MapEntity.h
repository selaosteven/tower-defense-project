// Acces Spatial
#ifndef MAPENTITY_H
#define MAPENTITY_H

#include <Entity.h>
#include <string>
#include <vector>
#include <iostream>
#include <array>

class MapEntity {
private:
    std::vector<Entity> lst_; // Liste des entités
    std::vector<std::string> map_; // la Map Physique
public:
    MapEntity(std::vector<Entity> lst,std::vector<std::string> map);

    std::vector<Entity> allWithinRange(std::array<float,2> center,float range);
};

#endif