#include "Sprite.h"

namespace Sprite{

Sprite::Sprite() : position_{0,0,0}{}

Sprite::Sprite(std::array<int, 3> &pos) : position_{pos}{}

Sprite::Sprite(int x, int y, int zIndex) : position_{x,y,zIndex}{}

Sprite::~Sprite(){}

void Sprite::draw(SDL_Surface &win) const {};

}