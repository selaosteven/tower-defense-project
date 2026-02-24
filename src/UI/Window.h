#ifndef WINDOW_H
#define WINDOW_H
#include <SDL.h>
#include <mutex>
#include <string>
#include <list>

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
private:
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
    Uint64 ticks_;
    std::list<Sprites::Sprite*> sprites_;
    std::list<Entity*> entities_;

protected:
    int win_width_;
    int win_height_;
    const Uint32 win_flags_;
    float delta_time_;
    float scale_;
    Point camera_position_;

public:
    Window();
    Window(int width, int height);
    Window(int width, int heightn, Uint32 flags);
    ~Window();

    int Create(void * args);
    werrors inputs();
    void loop();

    void addSprite(Sprites::Sprite *sprite);
    void addEntity(Entity *entity);

};

}
#endif