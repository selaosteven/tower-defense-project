#include <SDL.h>
#include <fstream>
#include <iostream>

#include "Map.h"


Map::Map(std::string name_map){

    { // Bloc de portée scope
        std::ifstream file(name_map);
        std::string line;
        int x = 0 ;
        std::map<char, Case> mapcharcase{
            {'T', Case::Tower},
            {'D', Case::Start},
            {'A', Case::End},
            {'.', Case::Void},
            {'#', Case::Wall},
            {'C', Case::Path}
        }; 
        while (std::getline(file, line)) {

            // On ajoute une nouvelle ligne vide
            map_.push_back(std::vector<Case>{});           
            for (char c : line) {
                int y = 0;
                if(c == 'C')
                    path_.push_back(Point(x,y));
                y++;
                map_.back().push_back(mapcharcase.at(c));
            }
            x++;
        }

    } // Les variables locales seront détruit a la fin

}

void Map::print() const {
    for(std::vector<Case> v : map_){
        for(Case c : v){
            switch (c) :
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

void Map::addTower() const {

}

void Map::addEnemy() const {
    
}

int Map::getWidth(){
    return map_[0].size();
}

int Map::getHeight(){
    return map_.size();
}
