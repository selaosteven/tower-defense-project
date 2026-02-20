#include "QuadTree/QuadTree.h"
#include "MapEntity.h"

using namespace std;

MapEntity::MapEntity(vector<Entity> lst, float width, float height) :
    lst_{lst}, width_{width}, height_{height} {}

vector<Entity> MapEntity::allWithinRange(Point center,float range){
    // center => cordonnée de la tour 
    // range => range de la tour

    // Construction de la map QuadTree
    QuadTree qt_map{Rectangle{width_/2,height_/2,width_,height_}};
    for(Entity e : lst_){ // Pour chaque ennemie
        Point p = e.getPosition();
        qt_map.insert(p);
    }

    qt_map.print();

    std::vector<Point> lst_pts = qt_map.query(center,range); // Recupere tous les points dans la range
    
    // qt_map.print();
    std::vector<Entity> res;

    for(Entity e : lst_){
        for(Point p : lst_pts){
            if(p == e.getPosition()){
                res.push_back(e);
            }
        }
    }

    return res;

}
