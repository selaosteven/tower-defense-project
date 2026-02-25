// Acces Spatial
#ifndef MAPENTITY_H
#define MAPENTITY_H

#include <string>
#include <vector>
#include <iostream>
#include <array>
#include "Entities/Entity.h"
#include "Map.h"

class MapEntity {
private:
    std::vector<Entity> lst_; // Liste des entités
    float width_;
    float height_;
public:
    MapEntity(std::vector<Entity> lst,float width, float height);
    std::vector<Entity> allWithinRange(Point center,float range);
};

#endif