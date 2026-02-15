#ifndef QUADTREE_H
#define QUADTREE_H
#include <memory>
#include <vector>
#include <iostream>

#include "QuadTree/Rectangle.h"
#include "QuadTree/Point.h"
#include "Entities/Entity.h"

class QuadTree {
private:
    Rectangle boundary_; // Dimension de la case 
    
    std::unique_ptr<QuadTree> topLeftTree_; // Coin Haut Gauche de la division QuadTree
    std::unique_ptr<QuadTree> topRightTree_; // Coin Haut Droite de la division QuadTree
    std::unique_ptr<QuadTree> botLeftTree_; // Coin Bas Gauche de la division QuadTree 
    std::unique_ptr<QuadTree> botRightTree_;// Coin Bas Droite de la division QuadTree 
    std::vector<Point> points_; // Liste de points dans la case

    static const int capacity_ = 1; // Capacité max de points d'une case avant de se subdiviser

    bool divided_; // Pour savoir si le rectangle est déjà divisé
public:
    QuadTree(Rectangle boundary);

    void subDivide();
    void insert(Point point);
    void print(int level = 0) const;
    std::vector<Point> query(Point center, float range);
};

#endif