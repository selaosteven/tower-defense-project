#include <SDL.h>
#include <fstream>
#include <iostream>
#include <map>

#include "Map.h"


Map::Map::Map(std::string name_map){

    { // Scope block to limit the lifetime of local variables
        std::ifstream file(name_map);
        std::string line;
        float x = 0 ;
        std::list<Point> inter_path;
        std::map<char, Case> mapcharcase{
            {'T', Case::Tower},
            {'D', Case::Start},
            {'A', Case::End},
            {'.', Case::Void},
            {'#', Case::Wall},
            {'C', Case::Path}
        }; 

        Point pD{0,0}; // Start point
        Point pA{0,0}; // End Point

        while (std::getline(file, line)) {
            float y = 0;
            // Add a new empty row to the map grid
            map_.push_back(std::vector<Case>{});           
            for (char c : line) {
                if(c == 'T'){
                    std::string key = std::to_string(int(x)) + "_" + std::to_string(int(y));
                    tower_lst_.insert({key,Point(x,y)});
                }

                if(c == 'C')
                    inter_path.push_back(Point(x,y));

                if(c == 'D')
                    pD = Point{x,y};

                if(c == 'A')
                    pA = Point{x,y};

                y++;
                map_.back().push_back(mapcharcase.at(c));
            }
            x++;
        }
        printTower();

        // Create path 
        path_.push_back(pD);

        bool found = true;

        while (found) {
            found = false;

            float ax = path_.back().getX();
            float ay = path_.back().getY();

            // Lambda that scans the entire inter_path list
            inter_path.remove_if([&](const Point& p){

                float bx = p.getX();
                float by = p.getY();

                if ((abs(ax - bx) == 1 && ay == by) || (abs(ay - by) == 1 && ax == bx)) {
                    path_.push_back(p);  // Add to the path
                    found = true;        // Neighbor found
                    return true;         // remove_if deletes this point
                }

                return false;            // Keep the point otherwise
            });
        }

        path_.push_back(pA);

    } // Local variables destroyed here

}

// Debug
void Map::Map::print() const {
    for(std::vector<Case> v : map_){
        for(Case c : v){
            switch (c){
                case Case::Tower :
                    std::cout << "Tower\n" << std::endl;
                    break;
                case Case::Void :
                    std::cout << "Void\n" << std::endl;
                    break;
                case Case::Path :
                    std::cout << "Path\n" << std::endl;
                    break;
                case Case::Start :
                    std::cout << "Start\n" << std::endl;
                    break;
                case Case::End :
                    std::cout << "End\n" << std::endl;
                    break;
                case Case::Wall :
                    std::cout << "Wall\n" << std::endl;
                    break;
            }
        }
    }

}

// Case Map::Map::getCase(float x, float y) {
   
// }

// Debug
void Map::Map::printTower() {
    for (auto& [key, point] : tower_lst_) {
        std::cout << "Tour " << key 
                << " -> (" << point.getX() << ", " << point.getY() << ")\n";
    }
}

// Debug
void Map::Map::printCase(Case c){
    switch (c){
        case Case::Tower :
            std::cout << "Tower\n" << std::endl;
            break;
        case Case::Void :
            std::cout << "Void\n" << std::endl;
            break;
        case Case::Path :
            std::cout << "Path\n" << std::endl;
            break;
        case Case::Start :
            std::cout << "Start\n" << std::endl;
            break;
        case Case::End :
            std::cout << "End\n" << std::endl;
            break;
        case Case::Wall :
            std::cout << "Wall\n" << std::endl;
            break;
    }
}


