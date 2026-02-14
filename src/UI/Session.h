#ifndef SESSION_H
#define SESSION_H

#include <Map.h>
#include <MapEntity.h>
#include <Tower.h>
#include <Enemy.h>

class Session {
private:
    MapEntity mapEntity_;
    Map map_;
    std::vector<Tower> lst_tower_;
    std::vector<Enemy> lst_enemy_;    

};


#endif