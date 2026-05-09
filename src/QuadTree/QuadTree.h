#ifndef QUADTREE_H
#define QUADTREE_H
#include <memory>
#include <vector>
#include <iostream>

#include "QuadTree/Rectangle.h"
#include "QuadTree/Point.h"
#include "Entities/Enemy.h"

// YouTube link to the quadtree algo: https://www.youtube.com/watch?v=OJxEcs0w_kE&t=503s
class QuadTree {
private:
    Rectangle boundary_; // Dimensions of cell
    
    std::unique_ptr<QuadTree> topLeftTree_;  // Top-left quadrant
    std::unique_ptr<QuadTree> topRightTree_; // Top-right quadrant
    std::unique_ptr<QuadTree> botLeftTree_;  // Bottom-left quadrant
    std::unique_ptr<QuadTree> botRightTree_; // Bottom-right quadrant
    std::vector<Enemy*> lst_enemy_; // List of points stored in this cell

    static const int capacity_ = 1; // Max number of points before subdivision

    bool divided_; // to see if the cell can be divided
public:
    QuadTree(Rectangle boundary);

    void subDivide();
    void clear();
    void insert(Enemy* e);
    void remove(Enemy* e);
    void print(int level = 0) const;
    std::vector<Enemy*> query(Point center, float range);
    
};

#endif