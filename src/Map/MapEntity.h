// Acces Spatial
#ifndef MAPENTITY_H
#define MAPENTITY_H

#include <string>
#include <vector>
#include <iostream>
#include <array>
#include <memory>

#include "QuadTree/QuadTree.h"
#include "Entities/Entity.h"
#include "Map.h"
#include "Entities/Tower.h"

namespace Map {
    

class MapEntity {
private:
    float width_;
    float height_;
    QuadTree qdtree_;
public:
    MapEntity(float width, float height);
    std::vector<Enemy*> allWithinRange(const Tower& t);
    std::vector<Enemy*> query(Point center, float range);
    void addEnemy(Enemy* e);
    void removeEnemy(Enemy* e);
    void clear();
};

}
#endif