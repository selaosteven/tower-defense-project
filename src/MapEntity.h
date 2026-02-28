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
    float width_;
    float height_;
    QuadTree qdtree_;
public:
    MapEntity(float width, float height);
    std::vector<Enemy> allWithinRange(Point center,float range);
    void addEnemy();
    void removeEnemy();
};

#endif