#ifndef MAP_H
#define MAP_H

#include <unordered_map>
#include <vector>
#include <string>
#include <list>

#include "QuadTree/Point.h"

enum class Case {
    Tower,
    Wall,
    Path,
    Void, 
    Start,
    End
};

class Map {
private:
    std::unordered_map<std::string, Point> tower_lst_; // key => (x_y) , value => Point (x,y)
public:
    std::vector<std::vector<Case>> map_;
private:
    std::list<Point> path_; // Chemin

public:
    Map(std::string name_map);
    int  getWidth();
    int  getHeight();
    void print() const;

};


#endif