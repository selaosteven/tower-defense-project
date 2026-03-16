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
    inline float  getWidth() {if (map_.empty()) return 0; return map_[0].size();}
    inline float  getHeight() {return map_.size();}
    inline std::list<Point> getPath() {return path_;}
    Case getCase(float x, float y);
    void print() const;
    void printTower();
    void printCase(Case c);
};

#endif