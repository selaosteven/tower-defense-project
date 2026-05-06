#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include "Map/Map.h"
#include "Map/MapEntity.h"

namespace UI
{
class Session : public Window{
private:
    Map::Map map_; // tower_lst_ , map_ , path_
    Map::MapEntity map_ope_; // lst_ , width_, height_
    int hp_player_;
    int round_;
    int money_;
    bool showUI_;
    bool waveActive_;
    int enemiesToSpawn_;
    float spawnTimer_;
public:
    Session(std::string name_map);
    void moneySetter(int new_money);
    void hpSetter(int new_hp);
    void mainSession();
    void startNextWave();
protected:
    void clickLeft(Point click) override;
    void drawUI(SDL_Renderer* r) override;

};


}
#endif