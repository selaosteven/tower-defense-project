#include <iostream>
#include "Entities/Enemy.h"
#include "Sprites/PrimitiveForm.h"

Enemy::Enemy(float lp, float speed, float resistance, bool fly) :
Entity{{0,0}}, lp_{lp}, speed_{speed}, resistance_{resistance}, fly_{fly}, path_{}, offset_{0.0f} {
    sprites_ = createSprites({255,255,255,255});
}

Enemy::Enemy(Point position, float offset, const Enemy& ref, std::list<Point>::iterator start, std::list<Point>::iterator end) 
    : Enemy{ref.lp_, ref.speed_, ref.resistance_, ref.fly_} {
    position_ = position;
    path_ = ++start;
    path_end_ = end;
    offset_ = offset;
}

void Enemy::live(float deltaTime) {
    if (!alive_ || reached_end_ || lp_ <= 0) return;

    // Process the stack of effects
    for (auto it = effects_.begin(); it != effects_.end(); ) {
        (*it)->apply(*this, deltaTime);
        if ((*it)->isExpired()) {
            it = effects_.erase(it);
        } else {
            ++it;
        }
    }

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
    if(dot >= len * (len + offset_)) {
        path_++;
        if (path_ == path_end_) reached_end_ = true;
    }
}

void Enemy::addEffect(std::unique_ptr<Effect> effect) {
    effects_.push_back(std::move(effect));
}


// static method

std::vector<Sprites::Sprite*> Enemy::createSprites(SDL_Color color) {
    // base : 
    Sprites::PrimitiveForm * Core = Sprites::triangle({0.0f,0.0f,1.0f}, 0.5);
    Sprites::PrimitiveForm * Behind = Sprites::rectangle({0.0f,0.0f,1.0f}, 0.5);

    return {Core, Behind};
}
