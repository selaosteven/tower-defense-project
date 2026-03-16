#include <SDL.h>
#include <fstream>
#include <iostream>
#include <map>

#include "Map.h"


Map::Map::Map(std::string name_map){

    { // Bloc de portée scope
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

        Point pD{0,0};
        Point pA{0,0};
        while (std::getline(file, line)) {
            float y = 0;
            // On ajoute une nouvelle ligne vide
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
        path_.push_back(pD);

        bool found = true;

        while (found) {
            found = false;

            float ax = path_.back().getX();
            float ay = path_.back().getY();

            // Lambda qui parcours toute la liste inter_path.
            inter_path.remove_if([&](const Point& p){

                float bx = p.getX();
                float by = p.getY();

                
                if ((abs(ax - bx) == 1 && ay == by) || (abs(ay - by) == 1 && ax == bx)) {
                    path_.push_back(p);  // ajoute au chemin
                    found = true;        // on a trouvé un voisin
                    return true;         // remove_if SUPPRIME ce point
                }

                return false;            // sinon on le garde
            });
        }

        path_.push_back(pA);

    } // Les variables locales seront détruit a la fin

}

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

Case Map::Map::getCase(float x, float y) {
   
}


void Map::Map::printTower() {
    for (auto& [key, point] : tower_lst_) {
        std::cout << "Tour " << key 
                << " -> (" << point.getX() << ", " << point.getY() << ")\n";
    }
}

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


