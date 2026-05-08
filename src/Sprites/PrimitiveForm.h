#ifndef PRIMITIVEFORM_H
#define PRIMITIVEFORM_H
#include <vector>
#include "Sprite.h"
#include "QuadTree/Point.h"
#include <memory>

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
    PrimitiveForm(const std::array<float, 3> &pos);
    PrimitiveForm(const std::array<float, 3> &pos, std::initializer_list<SDL_Vertex> points);
    PrimitiveForm(const std::array<float, 3> &pos, std::vector<SDL_Vertex> points);
    PrimitiveForm(const std::array<float, 3> &pos, std::initializer_list<SDL_FPoint> points, SDL_Color color);
    ~PrimitiveForm();

    inline void changeColor(SDL_Color color) {
        for(auto v : vertices_) v.color = color;
    }

    void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) override;
    friend std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float width, float height, SDL_Color color);
    friend std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float width, float height);
    friend std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float side, SDL_Color color);
    friend std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float side);
    std::shared_ptr<PrimitiveForm> circle(const std::array<float, 3> &pos, float size, int points);

    

};

std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float width, float height, SDL_Color color);
std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float width, float height);
std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float side, SDL_Color color);
std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float side);
std::shared_ptr<PrimitiveForm> circle(const std::array<float, 3> &pos, float size, int points = 20);
std::shared_ptr<PrimitiveForm> triangle(const std::array<float, 3> &pos, float size = -1, SDL_Color color = {255, 125, 30, 255}, Orientation orientation = Orientation::Up);
std::shared_ptr<PrimitiveForm> createColoredCircle(float radius, SDL_Color color, float zindex);
std::shared_ptr<PrimitiveForm> createCone(float radius, float angle_degrees, SDL_Color color, float zindex);

}

#endif