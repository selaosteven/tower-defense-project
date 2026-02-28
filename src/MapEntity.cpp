#include "QuadTree/QuadTree.h"
#include "MapEntity.h"

using namespace std;

MapEntity::MapEntity(float width, float height) :
    width_{width}, height_{height}, qdtree_{Rectangle{width_/2,height_/2,width_,height_}} {}

vector<Enemy> MapEntity::allWithinRange(Point center,float range){
    // center => cordonnée de la tour 
    // range => range de la tour

    std::vector<Point> lst_pts = qt_map.query(center,range); // Recupere tous les points dans la range
    
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

void MapEntity::addEnemy(Enemy e){
    qt_map.insert(e);
}

void MapEntity::removeEnemy(Enemy e){
    qt_map.remove(e);
}
