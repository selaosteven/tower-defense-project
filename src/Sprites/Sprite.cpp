#include <SDL.h>

#include "Sprite.h"

namespace Sprites
{

Sprite::Sprite() : position_{0,0,0}{}

Sprite::Sprite(std::array<int, 3> &pos) : position_{pos}{}

Sprite::Sprite(int x, int y, int zIndex) : position_{x,y,zIndex}{}

Sprite::~Sprite(){}

void Sprites::Sprite::draw(SDL_Renderer *win) const {
    SDL_SetRenderDrawColor(win,100,255,55,255);
    SDL_Rect objet = {position_[0],position_[1],21,21};
    SDL_RenderFillRect(win,&objet);
}
}