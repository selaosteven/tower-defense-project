// Acces Spatial
#ifndef MAPENTITY_H
#define MAPENTITY_H

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include "QuadTree/QuadTree.h"
#include "Entities/Entity.h"
#include "Map.h"

namespace Map {
    

class MapEntity {
private:
    float width_;
    float height_;
    QuadTree qdtree_;
public:
    MapEntity(float width, float height);
    std::vector<Enemy*> allWithinRange(Tower t);
    void addEnemy(Enemy* e);
    void removeEnemy(Enemy *e);
};

}
#endif