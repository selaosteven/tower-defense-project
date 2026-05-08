#ifndef BUTTON_H
#define BUTTON_H

#include "Sprites/Sprite.h"
#include <vector>
#include <memory>
#include <functional>

namespace Sprites {

class Button : public Sprite {
private:
    float width_;
    float height_;
    std::vector<std::shared_ptr<Sprite>> children_;

    std::function<void()> onLeftClick_;
    std::function<void()> onRightClick_;

public:
    Button(const std::array<float, 3>& pos, float width, float height);
    ~Button() override;

    // Takes ownership of a sprite and uses it as decoration
    void addSubSprite(std::shared_ptr<Sprite> sprite);

    void setOnLeftClick(std::function<void()> cb) { onLeftClick_ = cb; }
    void setOnRightClick(std::function<void()> cb) { onRightClick_ = cb; }
    
    void triggerLeftClick() { if (onLeftClick_) onLeftClick_(); }

    void draw(SDL_Renderer* win, float deltaTime, Point offset, float scale, float rot) override;
    bool onClick(Point click, int button, Point offset, float scale) override;
};

}
#endif
