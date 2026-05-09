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
    std::vector<Enemy*> allWithinRange(const Tower& t);/**
     * @brief return all enemies in range of tower t
     * 
     * @param center 
     * @param range 
     * @return std::vector<Enemy*> 
     */
    std::vector<Enemy*> query(Point center, float range);
    /**
     * @brief return all enemies in a circular area with a range
     * 
     *
     * @param center
     * @param range
     * @return std::vector<Enemy*>
     */
    void addEnemy(Enemy* e);
    void removeEnemy(Enemy* e);
    void clear();
};

}
#endif