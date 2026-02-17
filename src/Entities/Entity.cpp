#include "Entity.h"

Entity::Entity(Point position,float orientation) :
    position_{position} , orientation_{orientation} {}

Entity::Entity(Point position) :
    position_{position} , orientation_{0} {}
