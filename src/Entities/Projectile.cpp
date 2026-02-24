#include <vector>
#include "Entities/Projectile.h"
#include "Entities/Enemy.h"
#include "Entities/Augment.h"

void Projectile::do_hit(std::vector<Enemy*> enemies){

}

void Projectile::hit(std::vector<Enemy*> enemies){
    if (augments_) {
        for(auto a : *augments_) {
            a->projectile_hit_prefix(enemies, *this);
        }
    }
    do_hit(enemies);
    if (augments_) {
        for(auto a : *augments_) {
            a->projectile_hit_postfix(enemies, *this);
        }
    }
}


Projectile::Projectile(double size, double ps) : size_{size}, ps_{ps}, augments_{nullptr} {}