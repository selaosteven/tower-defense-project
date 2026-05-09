#ifndef ENEMY_BLUEPRINT_H
#define ENEMY_BLUEPRINT_H

#include <string>
#include <vector>
#include <memory>
#include "Entities/Enemy.h"
#include <SDL2/SDL.h>

class EnemyBlueprint {
private:
    std::string enemyType_; // type of enemy
    float baseLp_; // life point
    float baseSpeed_; // speed
    float baseResistance_; // resistance
    bool isFlying_; // flying or not
    std::string possibleChars_; // list of characters for the sprite
    SDL_Color color_; // color of the sprite

public:
    EnemyBlueprint();
    ~EnemyBlueprint();

    /**
     * @brief Loads the enemy blueprint from a JSON configuration file
     * 
     * @param position 
     * @param offset 
     * @param path_start 
     * @param path_end 
     * @param size 
     * @param round 
     * @return std::unique_ptr<Enemy> 
     */
    static std::unique_ptr<EnemyBlueprint> loadFromFile(const std::string& filepath);
    
    /**
     * @brief Creates an Enemy based on this blueprint
     * 
     * @param position
     * @param offset
     * @param path_start
     * @param path_end
     * @param size
     * @param round
     * @return 
     */
    std::unique_ptr<Enemy> instantiateEnemy(Point position, float offset, std::list<Point>::iterator path_start, std::list<Point>::iterator path_end, float size, int round) const;
    
    bool isFlying() const { return isFlying_; }
};

#endif
