#ifndef AUGMENT_H
#define AUGMENT_H
#include <vector>

#include "Entities/Effect.h"

class Projectile;
class Enemy;
class Tower;

class Augment
{
private:
    /* data */
public:
    Augment(/* args */);
    virtual ~Augment();

    // Methods 
public:
    virtual void projectile_hit_prefix(std::vector<Enemy*> enemies, Projectile& projectile);
    virtual void projectile_hit_postfix(std::vector<Enemy*> enemies, Projectile& projectile);

    // Before the tower shoot, it has a target but no projectile.
    // After the shot there is now a projectil created. 
    // ? keep or change this behaviour ? 
    virtual void tower_shoot_prefix(Tower& tower, Enemy& target);
    virtual void tower_shoot_postfix(Tower& tower, Enemy& target, Projectile& p);

    virtual void tower_rotate_prefix(Tower& tower, Enemy& target);
    virtual void tower_rotate_postfix(Tower& tower, Enemy& target);

    virtual void effect_apply_prefix(Effect& effect, Enemy& target);
    virtual void effect_apply_postfix(Effect& effect, Enemy& target);
};



#endif