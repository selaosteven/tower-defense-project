#include "MapEntity.h"

using namespace std;
MapEntity::MapEntity(vector<Entity> lst,vector<std::string> map) :
    lst_{lst}, map_{map} {}

vector<Entity> MapEntity::allWithinRange(array<float,2> center,float range){
    
}