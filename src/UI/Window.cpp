#include <iostream>
#include <string>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <SDL_ttf.h>

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
std::atomic<bool> Window::sdl_initiated{false};
unsigned int Window::number_of_instances = 0;
Uint32 Window::sdl_flags = 0;
std::mutex Window::event_mutex;


// ------------------------------------------------
//                  CONSTRUCTORS 
// ------------------------------------------------

UI::Window::Window() : Window{1368, 960, auto_flags_sdl_window}{}

UI::Window::Window(int width, int height) : Window{height, width, auto_flags_sdl_window} {}

UI::Window::Window(int width, int height, Uint32 flags) : renderer_(nullptr), window_(nullptr), event_(nullptr), thread_(nullptr), ticks_{0}, destroyed_{false}, sprites_{}, ui_sprites_{}, win_width_{width}, win_height_{height}, win_flags_{flags}, delta_time_{0}, scale_{1}, ui_scale_{1.0f}, camera_position_{0,0}, wants_to_die_{false} {
    number_of_instances++;
    std::string threadName = "SDL_WindowThread_" + std::to_string(number_of_instances);
    thread_ = SDL_CreateThread(Window::instanceWindowThread, threadName.c_str(), this);
}


UI::Window::~Window(){
    if (destroyed_) return;  // Prevent double deletion
    destroyed_ = true;
    
    // Wait for the window thread to finish if it's still running
    if (thread_ != nullptr) {
        int thread_return_value;
        SDL_WaitThread(thread_, &thread_return_value);
        thread_ = nullptr;
    }
    
    // Clean up SDL resources
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    if (event_ != nullptr) {
        delete event_;
        event_ = nullptr;
    }
    TTF_Quit();
    
    if (number_of_instances == 0 && sdl_initiated) {
        SDL_Quit();
        sdl_initiated = false;
    }
}
// ------------------------------------------------
//              FABRICATORS DESTRUCTOR 
// ------------------------------------------------

int UI::Window::Create(void * args){
    
    {
        std::lock_guard<std::mutex> lock(event_mutex);
        if(!Window::sdl_initiated) init_sdl();
    }
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


void UI::Window::waitForClose(){
    if (thread_ != nullptr) {
        int thread_return_value;
        SDL_WaitThread(thread_, &thread_return_value);
        thread_ = nullptr;
    }
}

// ------------------------------------------------
//             INPUT FUNCTIONS 
// ------------------------------------------------


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
                wants_to_die_ = true;
                SDL_PushEvent(event_);
                return STOP;

            case SDL_WINDOWEVENT:
                if (event_->window.windowID == SDL_GetWindowID(window_)) {
                    is_for_me = true;
                    if (event_->window.event == SDL_WINDOWEVENT_CLOSE) {
                        wants_to_die_ = true;
                        lock.unlock();
                        return STOP;
                    }
                    if (event_->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        win_width_ = event_->window.data1;
                        win_height_ = event_->window.data2;
                    }
                }
                break;
            case SDL_KEYDOWN:
                if (event_->key.windowID == SDL_GetWindowID(window_)) {
                    is_for_me = true;

                    switch(event_->key.keysym.sym) {

                        case SDLK_LEFT:
                            std::cout << "fleche gauche" << std::endl;
                            onArrowLeft();
                            break;

                        case SDLK_RIGHT:
                            std::cout << "fleche droite" << std::endl;
                            onArrowRight();
                            break;

                        case SDLK_UP:
                            std::cout << "fleche haut" << std::endl;
                            onArrowUp();
                            break;

                        case SDLK_DOWN:
                            std::cout << "fleche bas" << std::endl;
                            onArrowDown();
                            break;

                        case SDLK_RETURN:
                            std::cout << "validation clavier" << std::endl;
                            onValidateSelection();
                            break;
                        case SDLK_ESCAPE:
                            std::cout << "Escape" << std::endl;
                            onEscape();
                            break;
                            
                        case SDLK_SPACE:
                            std::cout << "SPACE" << std::endl;
                            onSpace();
                            break;
                        case SDLK_f:
                            game_speed_multiplier_ = game_speed_multiplier_ > 5 ? 1 : game_speed_multiplier_ == 0 ? 1 : (game_speed_multiplier_ + 2);
                            std::cout << "Game speed set to " << game_speed_multiplier_ << "x" << std::endl;
                            break;
                        case SDLK_g:
                            game_speed_multiplier_ = 0;
                            std::cout << "Game speed set to " << game_speed_multiplier_ << "x" << std::endl;
                            break;
                        default:
                            break;
                    }
                }
                break;

                

            case SDL_KEYUP:
                if (event_->key.windowID == SDL_GetWindowID(window_)){
                    is_for_me = true;
                }
                break;
            case SDL_MOUSEBUTTONDOWN: { // Clic de la souris qui vient d'être pressé
                if (event_->button.windowID == SDL_GetWindowID(window_)) is_for_me = true;
                
                Point click{static_cast<float>(event_->button.x),static_cast<float>(event_->button.y)};
                bool consumed = false;

                // Propagate click to UI sprites (highest Z-index first)
                {
                    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
                    for(auto it = ui_sprites_.rbegin(); it != ui_sprites_.rend(); ++it){
                        if (auto s = it->lock()) {
                            Point pos = s->getPosition();
                            float offsetX = (pos.getX() < 0) ? static_cast<float>(win_width_) : 0.0f;
                            float offsetY = (pos.getY() < 0) ? static_cast<float>(win_height_) : 0.0f;
                            if(s->onClick(click, event_->button.button, Point{offsetX, offsetY}, ui_scale_)) {
                                consumed = true;
                                break;
                            }
                        }
                    }
                }

                if (!consumed) {
                    Uint8 btn = event_->button.button;
                    if (btn <= 3) {
                        is_dragging_[btn] = true;
                        drag_start_pos_[btn] = click;
                    }
                    
                    if(event_->button.button == SDL_BUTTON_LEFT){ // Clic gauche
                        clickLeft(click);
                    }
                    else if(event_->button.button == SDL_BUTTON_RIGHT){ // Clic droit
                        clickRight(click);
                    }
                    else if(event_->button.button == SDL_BUTTON_MIDDLE){ // Clic molette
                        clickMiddle(click);
                    }
                }
                break;
            }
        
            case SDL_MOUSEBUTTONUP: // Clic de la souris qui vient d'être relaché
                if (event_->button.windowID == SDL_GetWindowID(window_)) {
                    is_for_me = true;
                    Uint8 btn = event_->button.button;
                    if (btn <= 3 && is_dragging_[btn]) {
                        Point drop_pos{static_cast<float>(event_->button.x), static_cast<float>(event_->button.y)};
                        onMouseDrop(drop_pos, drag_start_pos_[btn], btn);
                        is_dragging_[btn] = false;
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                if (event_->motion.windowID == SDL_GetWindowID(window_)) {
                    is_for_me = true;
                    Point current_pos{static_cast<float>(event_->motion.x), static_cast<float>(event_->motion.y)};
                    for (Uint8 i = 1; i <= 3; ++i) {
                        if (is_dragging_[i]) {
                            onMouseDrag(current_pos, drag_start_pos_[i], i);
                            drag_start_pos_[i] = current_pos;
                        }
                    }
                }
                break;
            case SDL_MOUSEWHEEL:
                if (event_->wheel.windowID == SDL_GetWindowID(window_)) {
                    is_for_me = true;
                    float scrollX = static_cast<float>(event_->wheel.x);
                    float scrollY = static_cast<float>(event_->wheel.y);
                    if (event_->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                        scrollX *= -1.0f;
                        scrollY *= -1.0f;
                    }
                    onMouseScroll(scrollX, scrollY);
                }
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

// ------------------------------------------------
//             DRAW FUNCTIONS 
// ------------------------------------------------


void UI::Window::loop(){
    while(1)
    {        
        SDL_SetRenderDrawColor(renderer_,0,0,0,255);
        SDL_RenderClear(renderer_);
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        delta_time_ = (SDL_GetTicks64() - ticks_) / 1000.0f;
        ticks_ = SDL_GetTicks64();
        delta_time_ *= game_speed_multiplier_;
        werrors errInputs = inputs();
        if(errInputs == STOP) break;
        
        {
            std::lock_guard<std::recursive_mutex> lock(render_mutex_);
            for(auto s : sprites_) s->draw(renderer_, delta_time_, camera_position_, scale_, 0);
            for(auto e : entities_) e->draw(renderer_, delta_time_, camera_position_, scale_, 0);
            
            drawSelection(renderer_);
            
            // Draw UI Elements fixed to the screen, anchoring to opposite sides if coordinate is negative
            for(auto it = ui_sprites_.begin(); it != ui_sprites_.end(); ) {
                if (auto s = it->lock()) {
                    Point pos = s->getPosition();
                    float offsetX = (pos.getX() < 0) ? static_cast<float>(win_width_) : 0.0f;
                    float offsetY = (pos.getY() < 0) ? static_cast<float>(win_height_) : 0.0f;
                    s->draw(renderer_, delta_time_, Point{offsetX, offsetY}, ui_scale_, 0);
                    ++it;
                } else {
                    it = ui_sprites_.erase(it);
                }
            }
            
            drawUI(renderer_);
        }
            
        SDL_RenderPresent(renderer_);
    }
    return;
}

void UI::Window::addSprite(std::shared_ptr<Sprites::Sprite> sprite){
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
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

void UI::Window::addUISprite(std::weak_ptr<Sprites::Sprite> sprite){
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
    auto sp = sprite.lock();
    if (!sp) return;

    if(ui_sprites_.empty()) {
        ui_sprites_.push_front(sprite);
    } else {
        for(auto it = ui_sprites_.begin(); it != ui_sprites_.end(); ++it){
            if (auto current = it->lock()) {
                if(current->zindex_ > sp->zindex_) {
                    ui_sprites_.insert(it, sprite);
                    return;
                }
            }
        }
        ui_sprites_.push_back(sprite);        
    }
}

void UI::Window::removeSprite(std::shared_ptr<Sprites::Sprite> sprite){
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
    if(!sprites_.empty()) {
        sprites_.remove(sprite);
    }
}

void UI::Window::removeEntity(Entity *entity){
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
    if(!entities_.empty()) {
        entities_.remove(entity);
    }        
    
}

void UI::Window::addEntity(Entity *entity){
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
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
