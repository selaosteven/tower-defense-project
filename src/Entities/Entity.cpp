#include <vector>
#include <cmath>
#include <algorithm>
#include "Entity.h"

static const float pi = std::acos(-1.0f);


Entity::Entity(Point position, float orientation, std::vector<Sprites::Sprite*> sprites) : position_{position}, orientation_{orientation}, sprites_{sprites} {}


Entity::Entity(Point position, float orientation, Sprites::Sprite* sprite) : Entity{position, orientation, std::vector<Sprites::Sprite*>{sprite}} {}
Entity::Entity(Point position, Sprites::Sprite* sprite) : Entity{position, 0, {sprite}} {}
Entity::Entity(Point position, float orientation) : Entity{position,orientation, {}} {}
Entity::Entity(Point position) : Entity{position,0.0f} {}

Entity::~Entity() {
    for(auto s : sprites_) {
        delete s;
    }
    sprites_.clear();
}

void Entity::live(float deltaTime){
    
}

void Entity::draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot) {
    offset += position_ * scale;
    for(auto s : sprites_){
        if(s) s->draw(win, deltaTime, offset, scale, rot + orientation_);
    }
}

void Entity::addSprite(Sprites::Sprite* sprite) {
    if (sprite) {
        sprites_.push_back(sprite);
    }
}

void Entity::removeSprite(Sprites::Sprite* sprite) {
    auto it = std::find(sprites_.begin(), sprites_.end(), sprite);
    if (it != sprites_.end()) {
        sprites_.erase(it);
    }
}
