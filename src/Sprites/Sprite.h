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

public:
    static constexpr float unit_size_pixels = 10;

protected:
    Point position_;
    float zindex;
    float scale_;
    float rotation_;

public:
    Sprite();
    Sprite(const std::array<float, 3> &position, float scale, float rotation);
    Sprite(const std::array<float, 3> &position);
    Sprite(float x, float y, float zIndex);
    virtual ~Sprite();

    virtual void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot);
    
    inline void setScale(const float scale) {
        scale_ = scale;
    }
    inline void move(Point velocity){
        position_ += velocity;
    }

    inline Point getPosition() {return position_;}

    friend class UI::Window;
};
}
#endif