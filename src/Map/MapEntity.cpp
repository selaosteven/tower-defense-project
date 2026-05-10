#include "QuadTree/QuadTree.h"
#include "MapEntity.h"


Map::MapEntity::MapEntity(float width, float height) :
    width_{width}, height_{height}, qdtree_{Rectangle{width_/2,height_/2,width_,height_}} {}

std::vector<std::shared_ptr<Enemy>> Map::MapEntity::allWithinRange(const Tower& t){
    // Store every point in range of the Tower
    return qdtree_.query(t.getPosition(), t.getRange());
}

std::vector<std::shared_ptr<Enemy>> Map::MapEntity::query(Point center, float range){
    return qdtree_.query(center, range);
}

void Map::MapEntity::addEnemy(std::shared_ptr<Enemy> e){
    qdtree_.insert(e);
}

void Map::MapEntity::removeEnemy(std::shared_ptr<Enemy> e){
    qdtree_.remove(e);
}

void Map::MapEntity::clear(){
    qdtree_.clear();
}
