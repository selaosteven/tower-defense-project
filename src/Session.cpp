#include "Session.h"



Session::Session(std::string name_map): 
    UI::Window{},
    map_{name_map},
    map_ope_{std::vector<Entity>{}, map_.getWidth(), map_.getHeight()},
    hp_player_{50},
    round_{0},
    money_{0}
    {}

void Session::moneySetter(int new_money) {
    hp_player_ = new_money;
}

void Session::hpSetter(int new_hp) {
    hp_player_ = new_hp;
}


void Session::mainSession(){

    bool running = true;
    SDL_Event e;
    while (running) {
       
    }

}