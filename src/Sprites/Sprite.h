#ifndef SPRITE_H
#define SPRITE_H
#include <array>
#include <iostream>
#include <SDL.h>

#include "UI/Window.h"
#include "QuadTree/Point.h"

namespace Sprites
{
class Sprite
{

// ----------------------------
// Static - classwide
public:
    static constexpr float unit_size_pixels = 10;

// ----------------------------
// Object elements
protected:
    Point position_;
    float zindex_;
    float scale_;
    float rotation_;
    float inner_circle_radius_;

// ----------------------------
// Constructors
public:
    Sprite();
    Sprite(const std::array<float, 3> &position, float scale, float rotation);
    Sprite(const std::array<float, 3> &position);
    Sprite(float x, float y, float zindex);
    virtual ~Sprite();


// ----------------------------
// Objects Functions

    /**
     * @brief Core drawing function to adapt to all sub draw type. Takes in the renderer and elapsed time.
     * Also takes in details for relative transformations about offset, rotation and scale.
     * 
     * @param win 
     * @param deltaTime 
     * @param offset 
     * @param scale 
     * @param rot 
     */
    virtual void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot);
    /**
     * @brief Click event to go through action on sprites, usefull for UI. return True if event was used.
     * False to let it go through.
     * 
     * @param click 
     * @param button 
     * @param offset 
     * @param scale 
     * @return true 
     * @return false 
     */
    virtual bool onClick(Point click, int button, Point offset, float scale) { return false; }
    

// ----------------------------
// Inline functions (get/set)
    inline void setScale(const float scale) {
        scale_ = scale;
    }

    inline Point getPosition() const {return position_;}

    friend class UI::Window;
};
}
#endif