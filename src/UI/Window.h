#ifndef WINDOW_H
#define WINDOW_H
#include <SDL.h>
#include <mutex>
#include <string>
#include <list>
#include <memory>

#include "QuadTree/Point.h"

namespace Sprites {
    class Sprite;
}
class Entity;

namespace UI
{

void print_sdl_error(const char * msg);

const Uint32 auto_flags_sdl_init = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS;
const Uint32 auto_flags_sdl_window = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
const Uint32 auto_flags_sdl_renderer = SDL_RENDERER_ACCELERATED;

enum werrors {
    NONE=0,
    SDL_ALREADY_INITIATED=-1,
    SDL_INIT_FAILED,
    STOP,
};
using werrors = enum werrors;

class Window
{


// Static - classwide
protected:
    static bool sdl_initiated;
    static unsigned int number_of_instances;
    static Uint32 sdl_flags;
    static int instanceWindowThread(void * window);
    static std::mutex event_mutex;
public: 
    static werrors init_sdl();
    static werrors init_sdl(Uint32 flags);
    static bool isThereAnInstance();

// ----------------------------
// Object elements
private:
    SDL_Renderer * renderer_;
    SDL_Window * window_;
    SDL_Event * event_;
    SDL_Thread * thread_;
    Uint64 ticks_;
    bool destroyed_;

protected:
    std::list<std::shared_ptr<Sprites::Sprite>> sprites_;
    std::list<std::weak_ptr<Sprites::Sprite>> ui_sprites_;
    std::list<Entity*> entities_;
    std::recursive_mutex render_mutex_;
    int win_width_;
    int win_height_;
    const Uint32 win_flags_;
    float delta_time_;
    float scale_;
    float ui_scale_;
    Point camera_position_;
    bool wants_to_die_;
    bool is_dragging_[4] = {false, false, false, false}; // Indices 1, 2, 3 correspond to SDL_BUTTON_LEFT, MIDDLE, RIGHT
    Point drag_start_pos_[4] = {Point{0.0f, 0.0f}, Point{0.0f, 0.0f}, Point{0.0f, 0.0f}, Point{0.0f, 0.0f}};

public:
    Window();
    Window(int width, int height);
    Window(int width, int heightn, Uint32 flags);
    ~Window();

    int Create(void * args);
    werrors inputs();
    void loop();
    void waitForClose();

    void addSprite(std::shared_ptr<Sprites::Sprite> sprite);
    void removeSprite(std::shared_ptr<Sprites::Sprite> sprite);
    void addUISprite(std::weak_ptr<Sprites::Sprite> sprite);
    void addEntity(Entity *entity);
    void removeEntity(Entity *entity);


    inline int getWinWidth() { return win_width_;}
    inline int getWinHeight() { return win_height_;}

    inline void setUIScale(float scale) { ui_scale_ = scale; }
    inline float getUIScale() const { return ui_scale_; }

    virtual void drawUI(SDL_Renderer*) {}
    virtual void drawSelection(SDL_Renderer* r) {}

protected:
    virtual void clickLeft(Point click) {}
    virtual void clickRight(Point click) {}
    virtual void clickMiddle(Point click) {}
    virtual void onMouseScroll(float scrollX, float scrollY) {}
    virtual void onMouseDrag(Point current_pos, Point start_pos, Uint8 button) {}
    virtual void onMouseDrop(Point drop_pos, Point start_pos, Uint8 button) {}
    virtual void onArrowLeft() {}
    virtual void onArrowRight() {}
    virtual void onArrowUp() {}
    virtual void onArrowDown() {}
    virtual void onValidateSelection() {}
    virtual void onEscape() {}
    virtual void onSpace() {}
    
    const std::list<std::shared_ptr<Sprites::Sprite>>& getSprites() const { return sprites_; }


};

}
#endif