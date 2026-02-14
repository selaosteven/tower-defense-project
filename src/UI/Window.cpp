#include "Window.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <mutex>
#include <thread>

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

UI::Window::Window(int width, int height, Uint32 flags) : renderer_(nullptr), window_(nullptr), event_(nullptr), ticks_{0},win_width_{height}, win_height_{width}, win_flags_{flags}, delta_time_{0} {
    if(!Window::sdl_initiated) init_sdl();
    number_of_instances++;
    std::string threadName = "SDL_WindowThread_" + std::to_string(number_of_instances);
    SDL_DetachThread(SDL_CreateThread(Window::instanceWindowThread, threadName.c_str(), this));
}


UI::Window::~Window(){
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    delete event_;
}

int UI::Window::Create(void * args){
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
    // On limite le nombre d'events traités par tour pour éviter une boucle infinie
    // si on se renvoie la balle (events) entre threads indéfiniment.
    int max_events = 10;
    while (max_events-- > 0)
    {
        // SDL_PollEvent n'est pas thread-safe, il faut protéger l'accès
        std::unique_lock<std::mutex> lock(event_mutex);
        if (SDL_PollEvent(event_) == 0) break;

        bool is_for_me = false;

        switch (event_->type)
        {
            case SDL_QUIT:
                // SDL_QUIT est global. On le remet dans la file pour que les autres fenêtres le voient aussi
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
            
            // Pour les inputs, SDL cible automatiquement la fenêtre qui a le focus
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event_->key.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (event_->button.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                break;
            case SDL_MOUSEMOTION:
                if (event_->motion.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                break;
            default:
                break;
        }

        if (!is_for_me) {
            // Ce n'est pas pour nous, on le rend à la file d'attente
            SDL_PushEvent(event_);
            lock.unlock();
            // On laisse la main aux autres threads pour qu'ils tentent de récupérer leur event
            std::this_thread::yield();
        }
    }
    return NONE;
}

void UI::Window::loop(){
    while(1)
    {
        delta_time_ = ticks_ - SDL_GetTicks64();
        ticks_ = SDL_GetTicks64();
        werrors errInputs = inputs();
        if(errInputs == STOP) break;
    }
    return;
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
        sdl_flags = flags;
        if(SDL_Init(sdl_flags) < 0){
            print_sdl_error("Failed to create window");
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