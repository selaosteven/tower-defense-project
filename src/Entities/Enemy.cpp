#include <iostream>
#include "Entities/Enemy.h"
#include "Sprites/PrimitiveForm.h"

Enemy::Enemy(float lp, float speed, float resistance, bool fly) :
Entity{{0,0}}, lp_{lp}, speed_{speed}, resistance_{resistance}, fly_{fly}, path_{}, offset_{0.0f} {
    sprites_ = createSprites({255,255,255,255});
}
Enemy::Enemy(Point position, float offset, const Enemy& ref, std::list<Point>::iterator start) : Enemy{ref} {
    position_ = position;
    path_ = ++start;
    offset_ = offset;
}

void Enemy::live(float deltaTime) {
    auto prev = path_;
    Point origin = *(--prev);
    Point target = *path_;
    Point direction = origin ^ target;
    Point velocity = direction | (speed_ * deltaTime);
    // std::cout << direction << " v: " << velocity << " position : " << position_ << std::endl; 
    position_ += velocity;

    Point traveled = origin ^ position_;
    float len = std::sqrt(direction.getX()*direction.getX() + direction.getY()*direction.getY());
    float dot = traveled.getX()*direction.getX() + traveled.getY()*direction.getY();
    if(dot >= len * (len + offset_)) path_++;
}


// static method

std::vector<Sprites::Sprite*> Enemy::createSprites(SDL_Color color) {
    // base : 
    Sprites::PrimitiveForm * Core = Sprites::triangle({0.0f,0.0f,1.0f}, 0.5);
    Sprites::PrimitiveForm * Behind = Sprites::rectangle({0.0f,0.0f,1.0f}, 0.5);

    return {Core, Behind};
}