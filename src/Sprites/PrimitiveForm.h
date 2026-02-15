#ifndef PRIMITIVEFORM_H
#define PRIMITIVEFORM_H
#include <vector>
#include "Sprite.h"

namespace Sprites
{

class PrimitiveForm : public Sprite
{
    private:
        std::vector<SDL_Vertex> vertices;

    public:
    PrimitiveForm();
    PrimitiveForm(std::array<int, 3> &pos);
    ~PrimitiveForm();

    void draw(SDL_Surface &win) const override;
};

enum orientation {
    up = 0,
    right,
    down,
    left
};
using orientation = enum orientation;

PrimitiveForm triangle(std::array<int, 3> &pos, int size, orientation orientation = up);
PrimitiveForm circle(std::array<int, 3> &pos, int size, int points = 20);
PrimitiveForm rectangle(std::array<int, 3> &pos, int width, int height);
PrimitiveForm rectangle(std::array<int, 3> &pos, int side);


}

#endif