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

namespace Map {
    

class MapEntity {
private:
    float width_;
    float height_;
    QuadTree qdtree_;
public:
    MapEntity(float width, float height);
    std::vector<std::shared_ptr<Enemy>> allWithinRange(const Tower& t);
    void addEnemy(std::shared_ptr<Enemy> e);
    void removeEnemy(std::shared_ptr<Enemy> e);
};

}
#endif