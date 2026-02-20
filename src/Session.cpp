#include "Session.h"



Session::Session(std::string name_map)
    : map_{name_map},
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

    if(SDL_Init(SDL_INIT_VIDEO) != 0) { // Active le module vidéo
        std::cerr << "Erreur : " << SDL_GetError() << "\n";
        return;
    }

    // Création de la fenetre de titre "TOWER DEFENSE"
    SDL_Window* window = SDL_CreateWindow("TOWER DEFENSE", 
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    800, 600,
    SDL_WINDOW_SHOWN
    );

    // Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}