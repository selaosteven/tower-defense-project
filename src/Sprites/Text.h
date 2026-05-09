#ifndef TEXT_H
#define TEXT_H

#include "Sprites/Sprite.h"
#include <SDL_ttf.h>
#include <string>

namespace Sprites {

class Text : public Sprite {
// ----------------------------
// Static - classwide
public:
    static inline const std::string POKETEXT{"../src/Ressources/PokemonClassic.ttf"};
    static inline const std::string POK1{"../src/Ressources/POKPIX1.TTF"};
// ----------------------------
// Object elements
private:
    std::string text_;
    TTF_Font* font_;
    SDL_Color color_;
    SDL_Texture* texture_;
    int width_;
    int height_;
    bool centered_;
    int maxWidth_;
    bool needs_update_;

// ----------------------------
// constructors
public:
    // Basic text constructor
    Text(const std::array<float, 3>& pos, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, bool centered = false);
    
    // Wrapped text constructor (defines the width the text box MUST take)
    Text(const std::array<float, 3>& pos, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, int maxWidth, bool centered = false);

    ~Text() override;

// ----------------------------
// Object functions
    /**
     * @brief Set the Text object; Use a flag to refresh the texture based on the thread calling (draw or gameloop)
     * 
     * @param text 
     * @param renderer 
     */
    void setText(const std::string& text, SDL_Renderer* renderer = nullptr);
    /**
     * @brief Set the Color object, use the same flag to refresh the texture as the setText
     * 
     * @param color 
     * @param renderer 
     */
    void setColor(SDL_Color color, SDL_Renderer* renderer = nullptr);
    void draw(SDL_Renderer* win, float deltaTime, Point offset, float scale, float rot) override;

private:
    void updateTexture(SDL_Renderer* renderer);

// ----------------------------
// Inline functions (get/set)
public:
    int getHeight() const { return height_; }
    int getWidth() const { return width_; }

};

}
#endif
