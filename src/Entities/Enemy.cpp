#include <iostream>
#include "Entities/Enemy.h"
#include "Sprites/PrimitiveForm.h"
#include "Sprites/Text.h"

Enemy::Enemy(float lp, float speed, float resistance, bool fly) :
    Enemy(lp, speed, resistance, fly, "A") {}

Enemy::Enemy(float lp, float speed, float resistance, bool fly, std::string display_char) :
    Enemy(lp, speed, resistance, fly, display_char, {255, 255, 0, 255}) {}

Enemy::Enemy(float lp, float speed, float resistance, bool fly, std::string display_char, SDL_Color color) :
    Enemy(lp, speed, resistance, fly, display_char, color, 0.7f) {}

Enemy::Enemy(float lp, float speed, float resistance, bool fly, std::string display_char, SDL_Color color, float size) :
    Entity{{0,0}, 0.0f, Enemy::createSprites(fly, display_char, color, size)}, lp_{lp}, speed_{speed}, resistance_{resistance}, fly_{fly}, display_char_{display_char}, path_{}, offset_{0.0f} {}

Enemy::Enemy(Point position, float offset, const Enemy& ref, std::list<Point>::iterator start, std::list<Point>::iterator end) 
    : Enemy{ref.lp_, ref.speed_, ref.resistance_, ref.fly_, ref.display_char_} {
    position_ = position;
    path_ = ++start;
    path_end_ = end;
    offset_ = offset;
}

void Enemy::live(float deltaTime) {
    if (!alive_ || reached_end_ || lp_ <= 0) return;

    // Process effects 
    for (auto it = effects_.begin(); it != effects_.end(); ) {
        (*it)->apply(*this, deltaTime);
        if ((*it)->isExpired()) {
            it = effects_.erase(it);
        } else {
            ++it;
        }
    }

    // Enemy follow the path
    auto prev = path_;
    Point origin = *(--prev);
    Point target = *path_;
    Point direction = origin ^ target;
    Point velocity = direction | (speed_ * deltaTime);
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

std::vector<std::shared_ptr<Sprites::Sprite>> Enemy::createSprites(bool is_flying, const std::string& display_char, SDL_Color color, float size) {

    auto core = std::make_shared<Sprites::Text>(std::array<float, 3>{0.0f, 0.0f, 1.0f}, display_char,
        Sprites::Text::POK1,
        24,
        color,
        true
    );
    
    core->setScale(size / 24.0f);
    
    return {core};
}