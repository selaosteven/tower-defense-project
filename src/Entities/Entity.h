#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include <vector>
#include <memory>
#include "QuadTree/Point.h"
#include "Sprites/Sprite.h"

class Entity{
protected:
    Point position_; // Position of entity
    float orientation_; // Orientation angle 
    std::vector<std::shared_ptr<Sprites::Sprite>> sprites_; // Renderable objects

    public:
    Entity(Point position);
    Entity(Point position, float orientation);
    Entity(Point position, std::shared_ptr<Sprites::Sprite> sprite);
    Entity(Point position, float orientation, std::shared_ptr<Sprites::Sprite> sprite);
    Entity(Point position, float orientation, std::vector<std::shared_ptr<Sprites::Sprite>> sprites);
    ~Entity(); 

    inline Point getPosition() const {return position_;}
    inline float getOrientation() const {return orientation_;}
    inline std::vector<std::shared_ptr<Sprites::Sprite>>& getSprites() {return sprites_;}

    /**
     * @brief add sprite to sprites_
     * 
     *
     * @param sprite 
     */
    void addSprite(std::shared_ptr<Sprites::Sprite> sprite);

    /**
     * @brief remove sprite to sprites_ 
     * 
     * @param sprite
     */
    void removeSprite(std::shared_ptr<Sprites::Sprite> sprite);
    
    /**
     * @brief Draw every sprites of the entity on the screen
     * 
     * @param win
     * @param deltaTime
     * @param offset
     * @param scale
     * @param rot
     */
     virtual void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot);
    
    /**
     * @brief Update the entity each frame
     * 
     * @param deltaTime
     */
     virtual void live(float deltaTime);
    
    
};


#endif