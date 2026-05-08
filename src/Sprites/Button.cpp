#include "Sprites/Button.h"
#include <SDL2/SDL.h>

namespace Sprites {

Button::Button(const std::array<float, 3>& pos, float width, float height)
    : Sprite{pos}, width_{width}, height_{height} {}

Button::~Button() {}

void Button::addSubSprite(std::shared_ptr<Sprite> sprite) {
    if (sprite) {
        children_.push_back(sprite);
    }
}

void Button::draw(SDL_Renderer* win, float deltaTime, Point offset, float scale, float rot) {
    offset += position_ * scale;
    
    for (auto& child : children_) {
        if (child) child->draw(win, deltaTime, offset, scale, rot + rotation_);
    }
}

bool Button::onClick(Point click, int button, Point offset, float scale) {
    offset += position_ * scale;

    float startX = offset.getX();
    float startY = offset.getY();
    float endX = startX + width_ * scale;
    float endY = startY + height_ * scale;

    if (click.getX() >= startX && click.getX() <= endX &&
        click.getY() >= startY && click.getY() <= endY) {
        
        if (button == SDL_BUTTON_LEFT && onLeftClick_) {
            onLeftClick_();
            return true;
        }
        if (button == SDL_BUTTON_RIGHT && onRightClick_) {
            onRightClick_();
            return true;
        }
        return true; // Still consume the event to prevent clicking through the UI
    }
    return false;
}

}
