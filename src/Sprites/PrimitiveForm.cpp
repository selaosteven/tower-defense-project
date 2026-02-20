#include <vector>
#include <cmath>
#include <map>

#include "Sprites/PrimitiveForm.h"
#include "QuadTree/Point.h"

namespace Sprites {


PrimitiveForm::PrimitiveForm() : Sprite{} {}

PrimitiveForm::PrimitiveForm(const std::array<int, 3> &pos, std::initializer_list<SDL_FPoint> points, SDL_Color color) :
Sprite{pos}, vertices_{}
{

    for(auto fp : points){
        vertices_.push_back({fp, color,{0,0}});
    }
}
PrimitiveForm::PrimitiveForm(const std::array<int, 3> &pos, std::initializer_list<SDL_Vertex> points) :
Sprite{pos}, vertices_{points}
{
}

PrimitiveForm::PrimitiveForm(const std::array<int, 3> &pos, std::vector<SDL_Vertex> points) :
Sprite{pos}, vertices_{std::move(points)}
{
}

PrimitiveForm::~PrimitiveForm(){}
void PrimitiveForm::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    static const float pi = std::acos(-1.0f);

    // Animation
    static const float rotVelocity = (2 * pi) / 4; // 360 degrée sur 4 seconde
    rotation_ += rotVelocity*deltaTime;
    std::vector<SDL_Vertex> transformed_vertices = vertices_;
    offset += position_;
    float cx = static_cast<float>(offset.getX());
    float cy = static_cast<float>(offset.getY());
    (void)rot;
    const float cosA = std::cos(rotation_);
    const float sinA = std::sin(rotation_);
    for(auto& v : transformed_vertices){
        float sx = v.position.x * scale;
        float sy = v.position.y * scale;
        float rx = sx * cosA - sy * sinA;
        float ry = sx * sinA + sy * cosA;
        v.position.x = cx + rx;
        v.position.y = cy + ry;
    }
    SDL_RenderGeometry(win, nullptr, transformed_vertices.data(), transformed_vertices.size(), nullptr, 0);
}


PrimitiveForm triangle(const std::array<int, 3> &pos, float size, SDL_Color color, Orientation orientation) {
    const float unit = Sprite::unit_size_pixels;
    if (size <= 0) size = unit;
    
    const float x = static_cast<float>(pos[0]);
    const float y = static_cast<float>(pos[1]);

    static const float pi = std::acos(-1.0f);

    static std::map<Orientation, const float> orientationToAngle = {
        {Orientation::Right, 0},
        {Orientation::Down, pi/2.0f},
        {Orientation::Left, pi},
        {Orientation::Up, -pi/2.0f},        
    }; 

    const float orientationAngle = orientationToAngle.at(orientation);

    auto get_vertex = [&](float angle_offset) -> SDL_Vertex {
        return {
            {std::cos(orientationAngle + angle_offset) * unit, std::sin(orientationAngle + angle_offset) * unit },
            color,
            {0, 0}
        };
    };

    SDL_Vertex A = get_vertex(0.0f);
    SDL_Vertex B = get_vertex(2.0f * pi / 3.0f);
    SDL_Vertex C = get_vertex(4.0f * pi / 3.0f);

    PrimitiveForm t{pos, {A, B, C}};
    t.setScale(size / unit);
    return t;
}

PrimitiveForm circle(const std::array<int, 3> &pos, float size,const int points){
    static const float pi = std::acos(-1.0f);
    const float unit = Sprite::unit_size_pixels;
    if (size <= 0) size = unit;

    SDL_Color color = {255,255,255,255};
    std::vector<SDL_Vertex> vertices; 
    const float r = size;
    const float bangle = 2*pi/ points;
    auto get_vertex = [&](const int it,const float angle) -> SDL_Vertex {
        return {
            {std::cos(it*-angle) * r, std::sin(it*-angle) * r },
            color,
            {0, 0}
        };
    };
    SDL_Vertex center{
            { 0, 0},
            color,
            {0, 0}
    };
    for(int i = 0; i < points; i++){
        vertices.push_back(center);
        vertices.push_back(get_vertex(i, bangle));
        vertices.push_back(get_vertex(i + 1, bangle));
    }
    PrimitiveForm c{pos,vertices};
    return c;
}

PrimitiveForm rectangle(const std::array<int, 3> &pos, float width, float height) {
    const float unit = Sprite::unit_size_pixels;
    if (width <= 0) width = unit;
    if (height <= 0) height = unit;
    
    SDL_Color color {255, 70, 25, 255};

    SDL_Vertex A {{-width/2.0f, height/2.0f}, color, {0, 0}};
    SDL_Vertex B {{width/2.0f, height/2.0f}, color, {0, 0}};
    SDL_Vertex C {{width/2.0f, -height/2.0f}, color, {0, 0}};
    SDL_Vertex D {{-width/2.0f, -height/2.0f}, color, {0, 0}};

    return PrimitiveForm{pos, {A, B, C, A, C, D}};
}

PrimitiveForm rectangle(const std::array<int, 3> &pos, float side) {
    return rectangle(pos, side, side);
}

}