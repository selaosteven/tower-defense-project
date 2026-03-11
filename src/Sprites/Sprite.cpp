#include <SDL.h>

#include "Sprite.h"
#include "QuadTree/Point.h"

namespace Sprites
{

Sprite::Sprite() : Sprite{0,0,0} {}

Sprite::Sprite(const std::array<float, 3> &pos) : Sprite{pos, 1, 0} {}

Sprite::Sprite(float x, float y, float zindex) : Sprite{{x,y,zindex}, 1, 0}{}

Sprite::Sprite(const std::array<float, 3> &position, float scale, float rotation) :
position_{position[0], position[1]}, zindex_{position[2]}, scale_{scale}, rotation_{rotation}, inner_circle_radius_{1.0f}
{

};

Sprite::~Sprite(){}

void Sprites::Sprite::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    SDL_SetRenderDrawColor(win,100,255,55,255);
    offset += position_;
    SDL_Rect objet = {(int)offset.getX(),(int)offset.getY(),21,21};
    SDL_RenderFillRect(win,&objet);
}

}