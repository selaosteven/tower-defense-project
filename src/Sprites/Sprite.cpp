#include <SDL.h>

#include "Sprite.h"

namespace Sprites
{

Sprite::Sprite() : position_{0,0,0}{}

Sprite::Sprite(const std::array<int, 3> &pos) : Sprite{pos, 1, 0} {}

Sprite::Sprite(int x, int y, int zIndex) : Sprite{{x,y,zIndex}, 1, 0}{}

Sprite::Sprite(const std::array<int, 3> &position, float scale, float rotation) :
position_{position}, scale_{scale}, rotation_{rotation}
{

};

Sprite::~Sprite(){}

void Sprites::Sprite::draw(SDL_Renderer *win) const {
    SDL_SetRenderDrawColor(win,100,255,55,255);
    SDL_Rect objet = {position_[0],position_[1],21,21};
    SDL_RenderFillRect(win,&objet);
}

}