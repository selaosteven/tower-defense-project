#include "QuadTree/QuadTree.h"
#include "MapEntity.h"


Map::MapEntity::MapEntity(float width, float height) :
    width_{width}, height_{height}, qdtree_{Rectangle{width_/2,height_/2,width_,height_}} {}

std::vector<Enemy*> Map::MapEntity::allWithinRange(Tower t){

    // Recupere tous les points dans la range
    return qdtree_.query(t);

}

void Map::MapEntity::addEnemy(Enemy* e){
    qdtree_.insert(e);
}

void Map::MapEntity::removeEnemy(Enemy* e){
    qdtree_.remove(e);
}
