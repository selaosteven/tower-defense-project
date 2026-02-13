#ifndef SPRITE_H
#define SPRITE_H
#include <array>
#include <iostream>
#include <SDL.h>

namespace Sprite
{
class Sprite
{


protected:
    std::array<int, 3> position_; // Position (x,y) of the sprite, Z-index for vertical alignement


public:
    Sprite();
    Sprite(std::array<int, 3> &position);
    Sprite(int x, int y, int zIndex);
    virtual ~Sprite();

    virtual void draw(SDL_Surface &win) const;
};
}
#endif