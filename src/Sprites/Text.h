#ifndef TEXT_H
#define TEXT_H

#include "Sprites/Sprite.h"
#include <SDL2/SDL_ttf.h>
#include <string>

namespace Sprites {

class Text : public Sprite {

public:
    static inline const std::string POKETEXT{"../src/Ressources/PokemonClassic.ttf"};

private:
    std::string text_;
    TTF_Font* font_;
    SDL_Color color_;
    SDL_Texture* texture_;
    int width_;
    int height_;
    bool centered_;
    int maxWidth_;

    void updateTexture(SDL_Renderer* renderer);

public:
    // Basic text constructor
    Text(const std::array<float, 3>& pos, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, bool centered = false);
    
    // Wrapped text constructor (defines the size the text box MUST take)
    Text(const std::array<float, 3>& pos, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, int maxWidth, bool centered = false);

    ~Text() override;

    void setText(const std::string& text, SDL_Renderer* renderer = nullptr);
    void setColor(SDL_Color color, SDL_Renderer* renderer = nullptr);

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    void draw(SDL_Renderer* win, float deltaTime, Point offset, float scale, float rot) override;
};

}
#endif
