#include "QuadTree/QuadTree.h"
#include "MapEntity.h"


Map::MapEntity::MapEntity(float width, float height) :
    width_{width}, height_{height}, qdtree_{Rectangle{width_/2,height_/2,width_,height_}} {}

std::vector<Enemy*> Map::MapEntity::allWithinRange(const Tower& t){
    // Recupere tous les points dans la range de la tour
    return qdtree_.query(t.getPosition(), t.getRange());
}

std::vector<Enemy*> Map::MapEntity::query(Point center, float range){
    return qdtree_.query(center, range);
}

void Map::MapEntity::addEnemy(Enemy* e){
    qdtree_.insert(e);
}

void Map::MapEntity::removeEnemy(Enemy* e){
    qdtree_.remove(e);
}

void Map::MapEntity::clear(){
    qdtree_.clear();
}
