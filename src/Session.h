#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include "Map.h"
#include "MapEntity.h"

class Session : public UI::Window{
private:
    Map map_; // tower_lst_ , map_ , path_
    MapEntity map_ope_; // lst_ , width_, height_
    int hp_player_;
    int round_;
    int money_;
public:
    Session(std::string name_map);
    void moneySetter(int new_money);
    void hpSetter(int new_hp);
    void mainSession();
    
protected:
    void clickLeft(Point click) override;

};


#endif