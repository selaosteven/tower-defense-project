#include "Sprites/Text.h"
#include <iostream>

namespace Sprites {

Text::Text(const std::array<float, 3>& pos, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, bool centered)
    : Text(pos, text, fontPath, fontSize, color, 0, centered) {}

Text::Text(const std::array<float, 3>& pos, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, int maxWidth, bool centered)
    : Sprite{pos}, text_{text}, font_{nullptr}, color_{color}, texture_{nullptr}, width_{0}, height_{0}, centered_{centered}, maxWidth_{maxWidth} 
{
    font_ = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font_) {
        std::cerr << "Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
    } else {
        // Calculate the full size of the text box immediately so the user can query it before drawing
        SDL_Surface* tempSurface = nullptr;
        if (maxWidth_ > 0) {
            tempSurface = TTF_RenderUTF8_Blended_Wrapped(font_, text_.c_str(), color_, maxWidth_);
        } else {
            tempSurface = TTF_RenderUTF8_Blended(font_, text_.c_str(), color_);
        }
        
        if (tempSurface) {
            width_ = tempSurface->w;
            height_ = tempSurface->h;
            SDL_FreeSurface(tempSurface);
        }
    }
}

Text::~Text() {
    if (texture_) SDL_DestroyTexture(texture_);
    if (font_) TTF_CloseFont(font_);
}

void Text::updateTexture(SDL_Renderer* renderer) {
    if (!font_ || text_.empty() || !renderer) return;

    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }

    SDL_Surface* surface = nullptr;
    if (maxWidth_ > 0) {
        surface = TTF_RenderUTF8_Blended_Wrapped(font_, text_.c_str(), color_, maxWidth_);
    } else {
        surface = TTF_RenderUTF8_Blended(font_, text_.c_str(), color_);
    }

    if (surface) {
        texture_ = SDL_CreateTextureFromSurface(renderer, surface);
        width_ = surface->w;
        height_ = surface->h;
        SDL_FreeSurface(surface);
    }
}

void Text::setText(const std::string& text, SDL_Renderer* renderer) {
    text_ = text;
    if (renderer) updateTexture(renderer);
    else {
        if (texture_) { SDL_DestroyTexture(texture_); texture_ = nullptr; }
    }
}

void Text::setColor(SDL_Color color, SDL_Renderer* renderer) {
    color_ = color;
    if (renderer) updateTexture(renderer);
}

void Text::draw(SDL_Renderer* win, float deltaTime, Point offset, float scale, float rot) {
    if (!font_ || text_.empty()) return;

    // Lazy initialization of the texture once we finally have the renderer
    if (!texture_) {
        updateTexture(win);
    }
    if (!texture_) return; 

    float px = position_.getX() * scale;
    float py = position_.getY() * scale;

    SDL_FRect destRect;
    destRect.w = width_ * scale_;
    destRect.h = height_ * scale_;
    
    destRect.x = offset.getX() + px - (centered_ ? (destRect.w / 2.0f) : 0);
    destRect.y = offset.getY() + py - (centered_ ? (destRect.h / 2.0f) : 0);

    SDL_RenderCopyF(win, texture_, nullptr, &destRect);
}

}
