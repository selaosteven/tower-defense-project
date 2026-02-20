#include <vector>
#include "Entities/Projectile.h"
#include "Entities/Enemy.h"
#include "Entities/Augment.h"

void Projectile::do_hit(std::vector<Enemy*> enemies){

}

void Projectile::hit(std::vector<Enemy*> enemies){
    for(auto a : augments_) {
        a->projectile_hit_prefix(enemies, *this);
    }
    do_hit(enemies);
    for(auto a : augments_) {
        a->projectile_hit_prefix(enemies, *this);
    }
}