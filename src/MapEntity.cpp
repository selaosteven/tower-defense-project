#include "MapEntity.h"
#include "QuadTree.h"

using namespace std;

MapEntity::MapEntity(vector<Entity> lst,vector<string> map) :
    lst_{lst}, map_{map} {}

vector<Entity> MapEntity::allWithinRange(Point center,float range){
    // center => cordonnée de la tour 
    // range => range de la tour

    int width =  map_[0].size(); // Largeur de la map
    int height = map_.size(); // Hauteur de la map

    // Construction de la map QuadTree
    QuadTree qt_map{Rectangle{width/2,height/2,width,height}};
    for(Entity e : lst_){ // Pour chaque ennemie
        Point p = e.getPosition();
        qt_map.insert(p);
    }

    qt_map.query(center,range); // Recupere tous les points dans la range

}