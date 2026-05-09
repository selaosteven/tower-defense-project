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

public:
    Sprite();
    Sprite(const std::array<float, 3> &position, float scale, float rotation);
    Sprite(const std::array<float, 3> &position);
    Sprite(float x, float y, float zindex);
    virtual ~Sprite();

    virtual void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot);
    
    // Returns true if the sprite "consumes" the click event
    virtual bool onClick(Point click, int button, Point offset, float scale) { return false; }
    

// ----------------------------
// Inline functions
    inline void setScale(const float scale) {
        scale_ = scale;
    }

    inline Point getPosition() const {return position_;}

    friend class UI::Window;
};
}
#endif