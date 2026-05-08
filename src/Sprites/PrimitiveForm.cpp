#include <vector>
#include <cmath>
#include <map>

#include "Sprites/PrimitiveForm.h"
#include "QuadTree/Point.h"

namespace Sprites {


PrimitiveForm::PrimitiveForm() : Sprite{} {}

PrimitiveForm::PrimitiveForm(const std::array<float, 3> &pos, std::initializer_list<SDL_FPoint> points, SDL_Color color) :
Sprite{pos}, vertices_{}
{

    for(auto fp : points){
        vertices_.push_back({fp, color,{0,0}});
    }
}
PrimitiveForm::PrimitiveForm(const std::array<float, 3> &pos, std::initializer_list<SDL_Vertex> points) :
Sprite{pos}, vertices_{points}
{
}

PrimitiveForm::PrimitiveForm(const std::array<float, 3> &pos, std::vector<SDL_Vertex> points) :
Sprite{pos}, vertices_{std::move(points)}
{
}

PrimitiveForm::~PrimitiveForm(){}
void PrimitiveForm::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {

    // Animation
    static const float rotVelocity = 0;// (2 * pi) / 4; // 360 degrée sur 4 seconde
    rotation_ += rotVelocity*deltaTime;
    std::vector<SDL_Vertex> transformed_vertices = vertices_;

    float cosRot = std::cos(rot);
    float sinRot = std::sin(rot);
    float px = position_.getX() * scale;
    float py = position_.getY() * scale;
    float cx = offset.getX() + (px * cosRot - py * sinRot);
    float cy = offset.getY() + (px * sinRot + py * cosRot);

    const float cosA = std::cos(rotation_ + rot);
    const float sinA = std::sin(rotation_ + rot);
    for(auto& v : transformed_vertices){
        float sx = v.position.x * scale * scale_;
        float sy = v.position.y * scale * scale_;
        float rx = sx * cosA - sy * sinA;
        float ry = sx * sinA + sy * cosA;
        v.position.x = cx + rx;
        v.position.y = cy + ry;
    }
    SDL_RenderGeometry(win, nullptr, transformed_vertices.data(), transformed_vertices.size(), nullptr, 0);
}



// std::shared_ptr<PrimitiveForm> createColoredCircle(float radius, SDL_Color color, float zindex) {
//     std::vector<SDL_Vertex> vertices;
//     const float pi = std::acos(-1.0f);
//     const int points = 60; // Higher point count for smooth big circles
//     const float bangle = 2.0f * pi / points;
//     SDL_Vertex center{{0.0f, 0.0f}, color, {0.0f, 0.0f}};
//     for (int i = 0; i < points; i++) {
//         vertices.push_back(center);
//         vertices.push_back({{static_cast<float>(std::cos(i * -bangle)) * radius, static_cast<float>(std::sin(i * -bangle)) * radius}, color, {0.0f, 0.0f}});
//         vertices.push_back({{static_cast<float>(std::cos((i + 1) * -bangle)) * radius, static_cast<float>(std::sin((i + 1) * -bangle)) * radius}, color, {0.0f, 0.0f}});
//     }
//     return std::shared_ptr<PrimitiveForm>(new PrimitiveForm({0.0f, 0.0f, zindex}, std::move(vertices)));
// }

std::shared_ptr<PrimitiveForm> createColoredCircle(
    float radius,
    SDL_Color color,
    float zindex
) {
    // zindex ignoré, pos = {0,0,0}
    return createColoredCircle(radius, color, zindex, {0.0f, 0.0f, 0.0f});
}

std::shared_ptr<PrimitiveForm> createColoredCircle(
    float radius,
    SDL_Color color,
    float zindex,                      // gardé mais ignoré
    const std::array<float, 3> &pos
) {
    std::vector<SDL_Vertex> vertices;
    const float pi = std::acos(-1.0f);
    const int points = 60;
    const float bangle = 2.0f * pi / points;

    // VERTICES EN LOCAL (0,0)
    SDL_Vertex center{{0.0f, 0.0f}, color, {0.0f, 0.0f}};

    for (int i = 0; i < points; i++) {
        vertices.push_back(center);

        vertices.push_back({
            {std::cos(i * -bangle) * radius,
             std::sin(i * -bangle) * radius},
            color,
            {0.0f, 0.0f}
        });

        vertices.push_back({
            {std::cos((i + 1) * -bangle) * radius,
             std::sin((i + 1) * -bangle) * radius},
            color,
            {0.0f, 0.0f}
        });
    }

    // pos = position du cercle dans le monde
    return std::make_shared<PrimitiveForm>(
        pos,
        std::move(vertices)
    );
}




std::shared_ptr<PrimitiveForm> createCone(float radius, float angle_degrees, SDL_Color color, float zindex) {
    std::vector<SDL_Vertex> vertices;
    const float pi = std::acos(-1.0f);
    int points = std::max(10, static_cast<int>(60 * angle_degrees / 360.0f));
    float half_angle = angle_degrees / 2.0f * pi / 180.0f;
    float step = (angle_degrees * pi / 180.0f) / points;
    SDL_Vertex center{{0.0f, 0.0f}, color, {0.0f, 0.0f}};
    for (int i = 0; i < points; i++) {
        vertices.push_back(center);
        float a1 = -half_angle + i * step;
        float a2 = -half_angle + (i + 1) * step;
        vertices.push_back({{static_cast<float>(std::cos(-a1)) * radius, static_cast<float>(std::sin(-a1)) * radius}, color, {0.0f, 0.0f}});
        vertices.push_back({{static_cast<float>(std::cos(-a2)) * radius, static_cast<float>(std::sin(-a2)) * radius}, color, {0.0f, 0.0f}});
    }
    return std::shared_ptr<PrimitiveForm>(new PrimitiveForm({0.0f, 0.0f, zindex}, std::move(vertices)));
}

std::shared_ptr<PrimitiveForm> triangle(const std::array<float, 3> &pos, float size, SDL_Color color, Orientation orientation) {
    const float unit = Sprite::unit_size_pixels;
    if (size <= 0) size = unit;
    
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

    auto t = std::shared_ptr<PrimitiveForm>(new PrimitiveForm{pos, {A, B, C}});
    t->setScale(size / unit);
    return t;
}

std::shared_ptr<PrimitiveForm> circle(const std::array<float, 3> &pos, float size,const int points){
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
    return std::shared_ptr<PrimitiveForm>(new PrimitiveForm{pos,vertices});
}

std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float width, float height, SDL_Color color) {
    const float unit = Sprite::unit_size_pixels;
    if (width <= 0) width = unit;
    if (height <= 0) height = unit;
    
    SDL_Vertex A {{-width/2.0f, height/2.0f}, color, {0, 0}};
    SDL_Vertex B {{width/2.0f, height/2.0f}, color, {0, 0}};
    SDL_Vertex C {{width/2.0f, -height/2.0f}, color, {0, 0}};
    SDL_Vertex D {{-width/2.0f, -height/2.0f}, color, {0, 0}};

    return std::shared_ptr<PrimitiveForm>(new PrimitiveForm{pos, {A, B, C, A, C, D}});
}
std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float width, float height) {
    return rectangle(pos, width, height, {125,255,30,255});
}

// carré
std::shared_ptr<PrimitiveForm> rectangle(const std::array<float, 3> &pos, float side) {
    return rectangle(pos, side, side);
}

std::shared_ptr<PrimitiveForm> octone(const std::array<float, 3> &pos, float size, SDL_Color color) {
    static const float pi = std::acos(-1.0f);
    const int sides = 8; // Octogone
    const float r = size;
    const float angleStep = 2.0f * pi / sides;

    std::vector<SDL_Vertex> vertices;
    SDL_Vertex center{{0.0f, 0.0f}, color, {0, 0}};

    for (int i = 0; i < sides; i++) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        SDL_Vertex v1{{std::cos(a1) * r, std::sin(a1) * r}, color, {0, 0}};
        SDL_Vertex v2{{std::cos(a2) * r, std::sin(a2) * r}, color, {0, 0}};

        vertices.push_back(center);
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    return std::shared_ptr<PrimitiveForm>(new PrimitiveForm(pos, std::move(vertices)));
}

std::shared_ptr<PrimitiveForm> hexagone(const std::array<float, 3> &pos, float size, SDL_Color color) {
    static const float pi = std::acos(-1.0f);
    const int sides = 6; // Hexagone
    const float r = size;
    const float angleStep = 2.0f * pi / sides;

    std::vector<SDL_Vertex> vertices;
    SDL_Vertex center{{0.0f, 0.0f}, color, {0, 0}};

    for (int i = 0; i < sides; i++) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        SDL_Vertex v1{{std::cos(a1) * r, std::sin(a1) * r}, color, {0, 0}};
        SDL_Vertex v2{{std::cos(a2) * r, std::sin(a2) * r}, color, {0, 0}};

        vertices.push_back(center);
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    return std::shared_ptr<PrimitiveForm>(new PrimitiveForm(pos, std::move(vertices)));
}

}