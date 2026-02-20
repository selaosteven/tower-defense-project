#include <SDL.h>

#include "Sprite.h"
#include "QuadTree/Point.h"

namespace Sprites
{

Sprite::Sprite() : Sprite{0,0,0} {}

Sprite::Sprite(const std::array<float, 3> &pos) : Sprite{pos, 1, 0} {}

Sprite::Sprite(float x, float y, float zIndex) : Sprite{{x,y,zIndex}, 1, 0}{}

Sprite::Sprite(const std::array<float, 3> &position, float scale, float rotation) :
position_{position[0], position[1]}, zindex{position[2]}, scale_{scale}, rotation_{rotation}
{

};

Sprite::~Sprite(){}

void Sprites::Sprite::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    SDL_SetRenderDrawColor(win,100,255,55,255);
    offset += position_;
    SDL_Rect objet = {offset.getX(),offset.getY(),21,21};
    SDL_RenderFillRect(win,&objet);
}

}