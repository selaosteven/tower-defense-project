#ifndef PRIMITIVEFORM_H
#define PRIMITIVEFORM_H
#include <vector>
#include "Sprite.h"

namespace Sprites
{


enum class Orientation {
    Up = 0,
    Right,
    Down,
    Left
};


class PrimitiveForm : public Sprite
{
    private:
        std::vector<SDL_Vertex> vertices_;

    public:
    PrimitiveForm();
    PrimitiveForm(const std::array<int, 3> &pos);
    PrimitiveForm(const std::array<int, 3> &pos, std::initializer_list<SDL_Vertex> points);
    PrimitiveForm(const std::array<int, 3> &pos, std::vector<SDL_Vertex> points);
    PrimitiveForm(const std::array<int, 3> &pos, std::initializer_list<SDL_FPoint> points, SDL_Color color);
    ~PrimitiveForm();

    void draw(SDL_Renderer *win) const override;

    friend PrimitiveForm triangle(const std::array<int, 3> &pos, float size, SDL_Color color, Orientation orientation);
    friend PrimitiveForm circle(const std::array<int, 3> &pos,const float size,const int points);
    friend PrimitiveForm rectangle(const std::array<int, 3> &pos, float side);
    friend PrimitiveForm rectangle(const std::array<int, 3> &pos, float width, float height);

};

PrimitiveForm rectangle(const std::array<int, 3> &pos, float width, float height);
PrimitiveForm rectangle(const std::array<int, 3> &pos, float side);
PrimitiveForm circle(const std::array<int, 3> &pos, float size, int points = 20);
PrimitiveForm triangle(const std::array<int, 3> &pos, float size = -1, SDL_Color color = {255, 125, 30, 255}, Orientation orientation = Orientation::Up);

}

#endif