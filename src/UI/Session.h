#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <memory>
#include <optional>
#include <map>
#include "Map/Map.h"
#include "Map/MapEntity.h"
#include "QuadTree/Point.h"
#include "Entities/TowerTree.h"
#include "Entities/Tower.h"
#include "Entities/EnemyBlueprint.h"
#include "Entities/Projectile.h"

struct UpgradeNode;

namespace Sprites {
    class Button;
}

namespace UI
{
class Session : public Window{
// ----------------------------
// Object element
private:
    Map::Map map_;
    Map::MapEntity map_ope_;
    int hp_player_;
    int round_;
    int money_;
    bool showUI_;
    bool waveActive_;
    int enemiesToSpawn_;
    float spawnTimer_;
    int ticks_per_seconds_;
    std::vector<Point> tower_build_cells_;
    std::vector<Point> tower_augment_cells;
    std::map<Point, std::vector<Tower*>> tac_towers;
    int tower_cursor_index_ = 0;

    // ----------------------------
    // Menu Navigation
    int menu_button_index_ = 0;
    std::vector<std::shared_ptr<Sprites::Button>> menu_buttons_;

    // ----------------------------
    // Tower UI Members
    std::optional<Point> selected_cell_;
    Tower* selected_tower_;
    std::vector<std::unique_ptr<TowerTree>> tower_catalog_;
    std::vector<std::unique_ptr<EnemyBlueprint>> enemy_catalog_;
    std::list<std::unique_ptr<Tower>> placed_towers_;
    std::list<std::unique_ptr<Tower>> sold_towers_;
    std::list<std::unique_ptr<Projectile>> active_projectiles_;
    std::vector<std::shared_ptr<Sprites::Sprite>> active_ui_elements_;
    // ----------------------------
    // UI Layout Settings
    float ui_panel_x_ = -350.0f;
    float ui_panel_y_ = 100.0f;
    float ui_panel_w_ = 300.0f;
    float ui_panel_h_ = 400.0f;

// ----------------------------
// constructors
public:
    Session(std::string name_map);
// ----------------------------
// inline function - (get/set)
    inline void moneySetter(int new_money){money_ = new_money;};
    inline void hpSetter(int new_hp) {hp_player_ = new_hp;}

// ----------------------------
// Gameplay loop / function
    void mainSession();
    void startNextWave();
private:
    void spawnEnemy(float cellSize, Point spawningDirection, float baseX, float baseY, std::list<Point>& path, std::vector<std::unique_ptr<Enemy>>& el);
    void GameOverScreen();

// ----------------------------
// event functions
protected:
    void clickLeft(Point click) override;
    /**
     * @brief With onArrowRight moves between tower cells.
     * 
     */
    void onArrowLeft() override;
    void onArrowRight() override;
    /**
     * @brief With onArrrowDown moves between active button of the ui open.
     * 
     */
    void onArrowUp() override;
    void onArrowDown() override;
    void onValidateSelection() override;
    /**
     * @brief Close the ui open
     * 
     */
    void onEscape() override;
    /**
     * @brief Start the next wave
     * 
     */
    void onSpace() override;
    /**
     * @brief Use middle or right click to move the camera
     * 
     * @param current_pos 
     * @param start_pos 
     * @param button 
     */
    void onMouseDrag(Point current_pos, Point start_pos, Uint8 button) override;
    /**
     * @brief Zoom in or out
     * 
     * @param scrollX 
     * @param scrollY 
     */
    void onMouseScroll(float scrollX, float scrollY) override;

// ----------------------------
// draw functions
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