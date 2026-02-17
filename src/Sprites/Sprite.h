#ifndef SPRITE_H
#define SPRITE_H
#include <array>
#include <iostream>
#include <SDL.h>

#include "UI/Window.h"

namespace Sprites
{
class Sprite
{

protected:
    static constexpr float unit_size_pixels = 10;

protected:
    std::array<int, 3> position_; // Position (x,y) of the sprite, Z-index for vertical alignement
    float scale_;
    float rotation_;

public:
    Sprite();
    Sprite(const std::array<int, 3> &position, float scale, float rotation);
    Sprite(const std::array<int, 3> &position);
    Sprite(int x, int y, int zIndex);
    virtual ~Sprite();

    virtual void draw(SDL_Renderer *win) const;
    
    inline void setScale(const float scale) {
        scale_ = scale;
    }

    friend class UI::Window;
};
}
#endif