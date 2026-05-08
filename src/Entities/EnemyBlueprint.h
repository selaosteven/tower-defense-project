#ifndef ENEMY_BLUEPRINT_H
#define ENEMY_BLUEPRINT_H

#include <string>
#include <vector>
#include <memory>
#include "Entities/Enemy.h"

class EnemyBlueprint {
private:
    std::string enemyType_;
    float baseLp_;
    float baseSpeed_;
    float baseResistance_;
    bool isFlying_;
    std::string possibleChars_;

public:
    EnemyBlueprint();
    ~EnemyBlueprint();

    // Loads the enemy blueprint from a JSON configuration file
    static std::unique_ptr<EnemyBlueprint> loadFromFile(const std::string& filepath);

    // Creates an Enemy based on this blueprint, ready for the map
    std::unique_ptr<Enemy> instantiateEnemy(Point position, float offset, std::list<Point>::iterator path_start, std::list<Point>::iterator path_end) const;
    
    // Accessors
    bool isFlying() const { return isFlying_; }
};

#endif
