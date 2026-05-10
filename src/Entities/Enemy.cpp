#include <iostream>
#include <algorithm>
#include "Entities/Enemy.h"
#include "Sprites/PrimitiveForm.h"
#include "Sprites/Text.h"
#include <memory>

Enemy::Enemy(float lp, float speed, float resistance, bool fly) :
    Enemy(lp, speed, resistance, fly, "A") {}

Enemy::Enemy(float lp, float speed, float resistance, bool fly, std::string display_char) :
    Enemy(lp, speed, resistance, fly, display_char, {255, 255, 0, 255}) {}

Enemy::Enemy(float lp, float speed, float resistance, bool fly, std::string display_char, SDL_Color color) :
    Enemy(lp, speed, resistance, fly, display_char, color, 0.7f) {}

Enemy::Enemy(float lp, float speed, float resistance, bool fly, std::string display_char, SDL_Color color, float size) :
    Entity{{0,0}, 0.0f, Enemy::createSprites(fly, display_char, color, size)}, lp_{lp}, max_lp_{lp}, speed_{speed}, resistance_{resistance}, fly_{fly}, display_char_{display_char}, path_{}, offset_{0.0f} {}

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

void Enemy::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    // Draw the base entity (the enemy sprite/text)
    Entity::draw(win, deltaTime, offset, scale, rot);

    // If it's alive
    if (lp_ <= 0 || max_lp_ <= 0) return;

    float hp_ratio = std::max(0.0f, std::min(1.0f, lp_ / max_lp_));

    float barW = 0.15f;
    float barH = 0.03f;
    float barY = -0.1f; // Floating slightly above the enemy
    
    // The background will always be the same
    static auto bg = Sprites::rectangle({0.0f, barY, 10.0f}, barW, barH, {80, 80, 80, 200});
    // Just need to be offset correctly
    Point my_offset = offset + position_ * scale;
    bg->draw(win, deltaTime, my_offset, scale, 0.0f);

    if (hp_ratio > 0.0f) {
        float fillW = barW * hp_ratio;
        float fillX = -barW / 2.0f + fillW / 2.0f; // Aligned to the left
        
        if(last_drawn_hp_ != lp_ || !hp_fill_sprite_){
            hp_fill_sprite_ = Sprites::rectangle({fillX, barY, 11.0f}, fillW, barH, {50, 255, 50, 255});
            last_drawn_hp_ = lp_;
        }

        if (hp_fill_sprite_)
            hp_fill_sprite_->draw(win, deltaTime, my_offset, scale, 0.0f);
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