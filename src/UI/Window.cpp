#include <iostream>
#include <string>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <SDL2/SDL_ttf.h>

#include "Window.h"
#include "Sprites/Sprite.h"
#include "Entities/Entity.h"

using namespace UI;


// ------------------------------------------------
//                  UTILS FUNCTIONS 
// ------------------------------------------------


void UI::print_sdl_error(const char * msg) {
    std::cerr << msg << "\n" << SDL_GetError() << std::endl;
    SDL_ClearError();
}

// ------------------------------------------------
//         STATIC VARIABLE INITIALIZATION
// ------------------------------------------------
bool Window::sdl_initiated = false;
unsigned int Window::number_of_instances = 0;
Uint32 Window::sdl_flags = 0;
std::mutex Window::event_mutex;


// ------------------------------------------------
//                  CONSTRUCTORS 
// ------------------------------------------------

UI::Window::Window() : Window{800, 600, auto_flags_sdl_window}{}

UI::Window::Window(int width, int height) : Window{height, width, auto_flags_sdl_window} {}

UI::Window::Window(int width, int height, Uint32 flags) : renderer_(nullptr), window_(nullptr), event_(nullptr), ticks_{0}, sprites_{}, win_width_{width}, win_height_{height}, win_flags_{flags}, delta_time_{0}, scale_{1}, camera_position_{0,0} {
    number_of_instances++;
    std::string threadName = "SDL_WindowThread_" + std::to_string(number_of_instances);
    SDL_DetachThread(SDL_CreateThread(Window::instanceWindowThread, threadName.c_str(), this));
}


UI::Window::~Window(){
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    TTF_Quit();
    delete event_;
}

int UI::Window::Create(void * args){
    
    std::mutex &lock(event_mutex);
    if(!Window::sdl_initiated) init_sdl();
    lock.unlock();
    bool failed = false;
    window_ = SDL_CreateWindow("Projet CPP SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_width_, win_height_, win_flags_);
    if(window_){
        renderer_ = SDL_CreateRenderer(window_, -1, auto_flags_sdl_renderer);
        if(!renderer_){
            renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
            if(!renderer_){
                print_sdl_error("Failed to create window");
                failed=true;
            }
        }
            if(!failed) event_ = new SDL_Event();
    }
    else {
        print_sdl_error("Failed to create window");
        failed=true;
    }
    if(failed) return -1;
    else loop();
    return 1;
}

werrors UI::Window::inputs(){
    int max_events = 10;
    while (max_events-- > 0)
    {
        std::unique_lock<std::mutex> lock(event_mutex);
        if (SDL_PollEvent(event_) == 0) break;

        bool is_for_me = false;

        switch (event_->type)
        {
            case SDL_QUIT:
                SDL_PushEvent(event_);
                return STOP;

            case SDL_WINDOWEVENT:
                if (event_->window.windowID == SDL_GetWindowID(window_)) {
                    is_for_me = true;
                    if (event_->window.event == SDL_WINDOWEVENT_CLOSE) {
                        lock.unlock();
                        return STOP;
                    }
                }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event_->key.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                break;
            case SDL_MOUSEBUTTONDOWN: // Clic de la souris qui vient d'être pressé
                if (event_->button.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                if(event_->button.button == SDL_BUTTON_LEFT){ // Clic gauche
                    Point click{static_cast<float>(event_->button.x),static_cast<float>(event_->button.y)};
                    clickLeft(click);
            
                    
                    break;
                }

                if(event_->button.button == SDL_BUTTON_RIGHT){ // Clic droit
                    std::cout << "clic droit | x : " << event_->button.x << " y : " << event_->button.y << "\n" << std::endl;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP: // Clic de la souris qui vient d'être relaché
                if (event_->button.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                break;
            case SDL_MOUSEMOTION:
                if (event_->motion.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                break;
            default:
                break;
        }

        if (!is_for_me) {
            SDL_PushEvent(event_);
            lock.unlock();
            std::this_thread::yield();
        }
    }
    return NONE;
}

void UI::Window::loop(){
    while(1)
    {        
        SDL_SetRenderDrawColor(renderer_,0,0,0,255);
        SDL_RenderClear(renderer_);
        delta_time_ = (SDL_GetTicks64() - ticks_) / 1000.0f;
        ticks_ = SDL_GetTicks64();
        werrors errInputs = inputs();
        if(errInputs == STOP) break;
        for(auto e : entities_) e->draw(renderer_, delta_time_, camera_position_, scale_, 0);
        for(auto s : sprites_) s->draw(renderer_, delta_time_, camera_position_, scale_, 0);
        
        drawUI(renderer_);
        SDL_RenderPresent(renderer_);
    }
    return;
}

void UI::Window::addSprite(Sprites::Sprite *sprite){
    if(sprites_.empty()) {
        sprites_.push_front(sprite);
    } else {
        for(auto it = sprites_.begin(); it != sprites_.end(); ++it){
            if((*it)->zindex_ > sprite->zindex_) {
                sprites_.insert(it, sprite);
                return;
            }
        }
        sprites_.push_back(sprite);        
    }
}


void UI::Window::removeEntity(Entity *entity){
    if(!entities_.empty()) {
        entities_.remove(entity);
    }        
    
}

void UI::Window::addEntity(Entity *entity){
    if(entities_.empty()) {
        entities_.push_front(entity);
    } else
        entities_.push_back(entity);        
    
}

// ------------------------------------------------
//             STATIC METHODS 
// ------------------------------------------------


int UI::Window::instanceWindowThread(void * window){
    Window* win = static_cast<Window*>(window);
    int res = win->Create(nullptr);
    delete win;
    number_of_instances--;
    return res;
}

werrors UI::Window::init_sdl() {
    return Window::init_sdl(auto_flags_sdl_init);
}
werrors UI::Window::init_sdl(Uint32 flags){
    if(!sdl_initiated){
        SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
        sdl_flags = flags;
        if(SDL_Init(sdl_flags) < 0){
            print_sdl_error("Failed to create window");
            return SDL_INIT_FAILED;
        }
        if (TTF_Init() == -1) {
            print_sdl_error("Failed to init TTF");
            return SDL_INIT_FAILED;
        }
        sdl_initiated = true;
        return NONE;
    }
    return SDL_ALREADY_INITIATED;
}

bool UI::Window::isThereAnInstance() {
    return number_of_instances > 0;
}

void UI::Window::clickLeft(Point click) {

    // float seuil = 10;

    // float px = position_.getX();
    // float py = position_.getY();

    // float dx = px - click.getX();
    // float dy = py - click.getY();

    // return (dx*dx + dy*dy <= seuil * seuil);
}