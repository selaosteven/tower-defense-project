#include "QuadTree/QuadTree.h"
#include "MapEntity.h"

using namespace std;

MapEntity::MapEntity(float width, float height) :
    width_{width}, height_{height}, qdtree_{Rectangle{width_/2,height_/2,width_,height_}} {}

vector<Enemy*> MapEntity::allWithinRange(Tower t){

    // Recupere tous les points dans la range
    return qdtree_.query(t);

}

void MapEntity::addEnemy(Enemy* e){
    qdtree_.insert(e);
}

void MapEntity::removeEnemy(Enemy* e){
    qdtree_.remove(e);
}
