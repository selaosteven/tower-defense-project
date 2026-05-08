#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <memory>
#include <optional>

#include "Map/Map.h"
#include "Map/MapEntity.h"
#include "QuadTree/Point.h"
#include "Entities/TowerTree.h"
#include "Entities/Projectile.h"

struct UpgradeNode;

namespace Sprites {
    class Button;
}

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
    int ticks_per_seconds_;
    std::vector<Point> tower_build_cells_;
    int tower_cursor_index_ = 0;

    // --- Menu Navigation ---
    int menu_button_index_ = 0;
    std::vector<std::shared_ptr<Sprites::Button>> menu_buttons_;

    // --- Tower UI Members ---
    std::optional<Point> selected_cell_;
    Tower* selected_tower_;
    std::vector<std::unique_ptr<TowerTree>> tower_catalog_;
    std::list<std::unique_ptr<Tower>> placed_towers_;
    std::list<std::unique_ptr<Projectile>> active_projectiles_;
    std::vector<std::shared_ptr<Sprites::Sprite>> active_ui_elements_;

public:
    Session(std::string name_map);
    void moneySetter(int new_money);
    void hpSetter(int new_hp);
    void mainSession();
    void startNextWave();

protected:
    void clickLeft(Point click) override;
    void onArrowLeft() override;
    void onArrowRight() override;
    void onArrowUp() override;
    void onArrowDown() override;
    void onValidateSelection() override;
    void drawUI(SDL_Renderer* r) override;
    void drawSelection(SDL_Renderer* r) override;


private:
    void openBuildUI(Point cell);
    void openUpgradeUI(Tower* tower);
    void closeTowerUI();
    void updateMenuButtonHighlight();
    static void drawHighlightBox(SDL_Renderer* r, float dt, Point offset, float scale, float x, float y, float w, float h, float thickness, SDL_Color col);
    

};


}
#endif