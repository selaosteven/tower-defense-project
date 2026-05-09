#include "Session.h"
#include <thread>

#include <filesystem>
#include <iostream>
#include <thread>
#include "Sprites/PrimitiveForm.h"
#include "Sprites/Text.h"
#include "Sprites/Button.h"
#include "Entities/Enemy.h"
#include "Entities/Tower.h"
#include "Entities/TowerTree.h"
#include "Entities/EnemyBlueprint.h"
#include "Entities/Projectile.h"
#include "QuadTree/QuadTree.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>


// ------------------------------------------------
//                  CONSTRUCTORS 
// ------------------------------------------------

UI::Session::Session(std::string name_map): 
    UI::Window{},
    map_{name_map},
    map_ope_{map_.getWidth(), map_.getHeight()},
    hp_player_{50},
    round_{0},
    money_{1000},
    showUI_{false},
    waveActive_{false},
    enemiesToSpawn_{0},
    spawnTimer_{0.0f},
    ticks_per_seconds_{120},
    selected_cell_{},
    selected_tower_{}
    {   
        const std::string tower_folder = "../src/Ressources/Towers";
        if (std::filesystem::exists(tower_folder)) {
            for (const auto& entry : std::filesystem::directory_iterator(tower_folder)) {
                if (entry.path().extension() == ".json") {
                    if (auto tower = TowerTree::loadFromFile(entry.path().string())) {
                        tower_catalog_.push_back(std::move(tower));
                    }
                }
            }
        }

        const std::string enemy_folder = "../src/Ressources/Enemies";
        if (std::filesystem::exists(enemy_folder)) {
            for (const auto& entry : std::filesystem::directory_iterator(enemy_folder)) {
                if (entry.path().extension() == ".json") {
                    if (auto enemy = EnemyBlueprint::loadFromFile(entry.path().string())) {
                        enemy_catalog_.push_back(std::move(enemy));
                    }
                }
            }
        }
        
        wave_configs_ = EnemyBlueprint::loadWaveCreator("../src/Ressources/waveCreator.json");
    }


// ------------------------------------------------
//                  EVENTS FUNCTIONS 
// ------------------------------------------------

void UI::Session::clickLeft(Point click) {
    // If the UI is open, clicks on UI buttons are handled by the buttons themselves.
    // We only need to check for clicks *outside* the UI panel to close it.
    if(hp_player_ <= 0) return;
    if (showUI_) {
        float offsetX = (ui_panel_x_ < 0) ? getWinWidth() : 0.0f;
        float offsetY = (ui_panel_y_ < 0) ? getWinHeight() : 0.0f;

        SDL_Rect uiRect = {
            static_cast<int>(offsetX + ui_panel_x_ * ui_scale_),
            static_cast<int>(offsetY + ui_panel_y_ * ui_scale_),
            static_cast<int>(ui_panel_w_ * ui_scale_),
            static_cast<int>(ui_panel_h_ * ui_scale_)
        };

        if (click.getX() < uiRect.x || click.getX() > uiRect.x + uiRect.w ||
            click.getY() < uiRect.y || click.getY() > uiRect.y + uiRect.h)
        {
            closeTowerUI();
            return;
        }
        return;
    }
    
    // UI is not open, handle world clicks

    float clickLX = (click.getX() - camera_position_.getX()) / scale_;
    float clickLY = (click.getY() - camera_position_.getY()) / scale_;

    int cellX = static_cast<int>(std::floor(clickLX));
    int cellY = static_cast<int>(std::floor(clickLY));

    if (cellY < 0 || cellY >= map_.getHeight() || cellX < 0 || cellX >= map_.getWidth() || map_.map_[cellY][cellX] != Case::Tower) {
        return;
    }

    std::shared_ptr<Tower> clicked_tower = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(render_mutex_);
        for (auto const& tower : placed_towers_) {
            Point tower_pos = tower->getPosition();
            if (static_cast<int>(std::floor(tower_pos.getX())) == cellX && static_cast<int>(std::floor(tower_pos.getY())) == cellY) {
                clicked_tower = tower;
                break;
            }
        }
    }

    if (clicked_tower) {
        openUpgradeUI(clicked_tower);
    } else {
        openBuildUI({(float)cellX, (float)cellY});
    }
}

void UI::Session::onArrowRight(){
    if (tower_build_cells_.empty()) return;
    tower_cursor_index_ = (tower_cursor_index_ + 1) % tower_build_cells_.size();
    selected_cell_ = tower_build_cells_[tower_cursor_index_];
}

void UI::Session::onArrowLeft(){
    if (tower_build_cells_.empty()) return;
    tower_cursor_index_ = (tower_cursor_index_ - 1) % tower_build_cells_.size();
    selected_cell_ = tower_build_cells_[tower_cursor_index_];
}

void UI::Session::onValidateSelection() {
    // If UI menu is open, trigger the selected button
    if (!menu_buttons_.empty()) {
        menu_buttons_[menu_button_index_]->triggerLeftClick();
        return;
    }
    
    // skip if the player's dead.
    if (!selected_cell_ || hp_player_ <= 0) return;

    float screenX = selected_cell_->getX() * scale_ + camera_position_.getX() + scale_ * 0.5f;
    float screenY = selected_cell_->getY() * scale_ + camera_position_.getY() + scale_ * 0.5f;

    clickLeft(Point{screenX, screenY});
}

void UI::Session::onArrowUp() {
    if (menu_buttons_.empty()) return;
    
    menu_button_index_ = (menu_button_index_ - 1 + menu_buttons_.size()) % menu_buttons_.size();
}

void UI::Session::onArrowDown() {
    if (menu_buttons_.empty()) return;
    
    menu_button_index_ = (menu_button_index_ + 1) % menu_buttons_.size();
}

void UI::Session::onEscape(){
    if(showUI_) closeTowerUI();
}

void UI::Session::onSpace(){
    startNextWave();
}

void UI::Session::onMouseDrag(Point current_pos, Point start_pos, Uint8 button) {
    if (button == SDL_BUTTON_RIGHT || button == SDL_BUTTON_MIDDLE) {
        camera_position_ += (current_pos - start_pos);
    }
}

void UI::Session::onMouseScroll(float scrollX, float scrollY) {
    if (scrollY == 0) return;

    float old_scale = scale_;
    float zoom_factor = 1.1f;

    if (scrollY > 0) scale_ *= zoom_factor;
    else scale_ /= zoom_factor;

    // Limit the zoom;
    scale_ = std::max(5.0f, std::min(scale_, 300.0f));

    // Center the zoom effect on the center of the camera.
    float cx = getWinWidth() / 2.0f;
    float cy = getWinHeight() / 2.0f;

    float worldX = (cx - camera_position_.getX()) / old_scale;
    float worldY = (cy - camera_position_.getY()) / old_scale;

    camera_position_ = Point{cx - worldX * scale_, cy - worldY * scale_};
}


// ------------------------------------------------
//                  DRAW FUNCTIONS 
// ------------------------------------------------

void UI::Session::generateMapSprites(){
    
    for(int y = 0; y < map_.getHeight(); y++) {
        for(int x = 0; x < map_.getWidth(); x++) {
            Case bloc = map_.map_.at(y).at(x);
            std::shared_ptr<Sprites::Sprite> s = nullptr;

            float px = x * cellSize + cellSize / 2.0f;
            float py = y * cellSize + cellSize / 2.0f;

            switch (bloc) {
                case Case::Tower: {
                    int r = rand() % 10;
                    if(r < 3){
                        s = Sprites::createColoredCircle(cellSize / 4,SDL_Color{255, 255, 0, 255},  99.0f,{px, py, 99.0f});
                        tower_augment_cells.push_back(Point{(float)x, (float)y});
                        tac_towers.insert({Point{(float)x, (float)y}, {}});
                        
                    } else {
                        s = Sprites::createColoredCircle(cellSize / 4,SDL_Color{255, 255, 255, 255},  99.0f,{px, py, 99.0f});
                    }
                    tower_build_cells_.push_back(Point{(float)x, (float)y});

                    break;
                }
                case Case::Path:
                    s = Sprites::rectangle({px, py, 99}, cellSize, cellSize, {70,70,70,255});
                    break;
                case Case::Wall:
                    break;
                case Case::Start:
                    s = Sprites::triangle({px, py, 99}, cellSize/4, {255, 125, 30, 255}, Sprites::Orientation::Left);
                    break;
                case Case::End:
                    s = Sprites::triangle({px, py, 99}, cellSize/4, {30, 160, 255, 255}, Sprites::Orientation::Right);
                    break;
                
                default:
                    s = Sprites::rectangle({px, py, 99}, cellSize/8,cellSize/8,(SDL_Color){125,80,125,200});
                    break;
            }

            if (s) addSprite(s); 
        }
    }
}

void UI::Session::closeTowerUI() {
    active_ui_elements_.clear();
    menu_buttons_.clear();
    menu_button_index_ = 0;
    if (auto st = selected_tower_.lock()) {
        st->setShowRange(false);
        st->setShowCone(false);
    }
    showUI_ = false;
    selected_tower_.reset();
    selected_cell_.reset();
}

void UI::Session::openBuildUI(Point cell) {
    if (showUI_) closeTowerUI(); // Close any existing UI first

    showUI_ = true;
    selected_cell_ = cell;
    menu_button_index_ = 0;
    menu_buttons_.clear();
    
    // We setup the ui settings for the draw;
    float startY = ui_panel_y_ + 10.0f;
    float stepY = 60.0f;
    float margin = 20.0f;
    // We use this width to fix all button with text;
    // we then use their height to adjust the step for the next one
    float buttonWidth = ui_panel_w_ - 2 * margin; 


    // Title
    auto title = std::make_shared<Sprites::Text>(std::array<float, 3>{ui_panel_x_ + 10.0f, startY, 11.0f}, "Build Tower", Sprites::Text::POKETEXT, 24, SDL_Color{255, 255, 255, 255});
    active_ui_elements_.push_back(title);
    addUISprite(title);
    startY += 40;
    
    // For each tower of the catalog we add a row.
    for (const auto& blueprint : tower_catalog_) {
        int cost = blueprint->getRootUpgrade() ? blueprint->getRootUpgrade()->cost : 0;
        std::string label = blueprint->getTowerType() + " (" + std::to_string(cost) + "$)";

        auto button = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, 50.0f);
        button->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, 25.0f, 0.0f}, buttonWidth, 50.0f, {80, 80, 150, 255}));
        
        auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, label, Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
        button->addSubSprite(text);

        // We add a lambda function on the button that will trigger the creation of the tower
        button->setOnLeftClick([this, blueprint = blueprint.get(), cost, cell = *selected_cell_]() {
            if (money_ >= cost) {
                money_ -= cost;
                
                float logicX = cell.getX() + 0.5f;
                float logicY = cell.getY() + 0.5f;

                Projectile dummyProj(-0.1f, 5.0f); // Default projectile have negative size to prevent AOE effect.
                auto new_tower = blueprint->instantiateTower({logicX, logicY}, dummyProj);
                
                addEntity(new_tower.get());
                {
                    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
                    placed_towers_.push_back(std::move(new_tower));
                }

                std::cout << "Built a " << blueprint->getTowerType() << " at " << logicX << ", " << logicY << std::endl;
                checkAddAugmentedCellBonus();
                closeTowerUI();
            } else {
                std::cout << "Not enough money!" << std::endl;
            }
        });

        active_ui_elements_.push_back(button);
        menu_buttons_.push_back(button);
        addUISprite(button);
        startY += stepY;
    }
    
    // We had a close button on the bottom of the list.

    auto buttonClose = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, 50.0f);
    buttonClose->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, 25.0f, 0.0f}, buttonWidth, 50.0f, {80, 80, 150, 255}));
    auto textClose = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, "CLOSE", Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
    buttonClose->addSubSprite(textClose);

    buttonClose->setOnLeftClick([this]() {
        closeTowerUI();
    });

    active_ui_elements_.push_back(buttonClose);
    menu_buttons_.push_back(buttonClose);
    addUISprite(buttonClose);

    startY += 50.0f + 10.0f;
    ui_panel_h_ = (startY - ui_panel_y_);

    // We add the background last but he has negative zindex so he will be drawn first with everything on top
    auto bg = Sprites::rectangle({ui_panel_x_ + ui_panel_w_ / 2.0f, ui_panel_y_ + ui_panel_h_ / 2.0f, -10.0f}, ui_panel_w_, ui_panel_h_, {20, 20, 40, 200});
    active_ui_elements_.push_back(bg);
    addUISprite(bg);

}

void UI::Session::openUpgradeUI(std::shared_ptr<Tower> tower) {
    if (showUI_) closeTowerUI();

    // Show tower range and cone when clicked
    showUI_ = true;
    selected_tower_ = tower;
    tower->setShowRange(true);
    tower->setShowCone(true);

    //We clear buttons and prepare to select the first.
    menu_button_index_ = 0;
    menu_buttons_.clear();

    // UI settings
    float startY = ui_panel_y_ + 10.0f;
    float stepY = 20.0f;
    float margin = 20.0f;
    // We use this width to fix all button with text;
    // we then use their height to adjust the step for the next one
    float buttonWidth = ui_panel_w_ - 2 * margin;
    int max_width_text = buttonWidth - 30; // padding of 15px per side

    auto title = std::make_shared<Sprites::Text>(std::array<float, 3>{ui_panel_x_ + 10.0f, startY, 11.0f}, tower->getType(), Sprites::Text::POKETEXT, 24, SDL_Color{255, 255, 255, 255});
    active_ui_elements_.push_back(title);
    addUISprite(title);
    startY += 40;

    // We add the close button first in contradiction to the buy UI to prevent missclick with the sell button
    auto buttonClose = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, 50.0f);
    buttonClose->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, 25.0f, 0.0f}, buttonWidth, 50.0f, {80, 80, 150, 255}));
    auto textClose = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, "CLOSE", Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
    buttonClose->addSubSprite(textClose);
    buttonClose->setOnLeftClick([this]() {
        closeTowerUI();
    });
    active_ui_elements_.push_back(buttonClose);
    menu_buttons_.push_back(buttonClose);
    addUISprite(buttonClose);
    startY += 50.0f + stepY;
    
    // We gather the current upgrade node of the tower 
    // and for each children we add a button with a lambda function to upgrade with the augment.
    const UpgradeNode* current_node = tower->getCurrentUpgradeNode();

    // If there's not upgrade available
    if (!current_node || current_node->children.empty()) {
        auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{ui_panel_x_ + margin, startY, 1.0f}, "No upgrades available.", Sprites::Text::POKETEXT, 14, SDL_Color{255, 255, 255, 255});
        active_ui_elements_.push_back(text);
        addUISprite(text);
        startY += 40.0f;
    } 
    else {        
        // else loop
        for (const auto& upgrade_node_ptr : current_node->children) {
            const UpgradeNode* upgrade = upgrade_node_ptr.get();
            std::string label = upgrade->name + " (" + std::to_string(static_cast<int>(upgrade->cost)) + "$)";
            auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, label, Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255}, max_width_text);
            float buttonHeight = std::max(50.0f, static_cast<float>(text->getHeight() + 30.0f));
            auto button = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, buttonHeight);

            button->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, buttonHeight / 2.0f, 0.0f}, buttonWidth, buttonHeight, {80, 80, 150, 255}));
            button->addSubSprite(text);
            button->setOnLeftClick([this, upgrade]() {
                if (money_ >= upgrade->cost) {
                    money_ -= upgrade->cost;
                    if (auto t = selected_tower_.lock()) {
                        t->applyUpgrade(upgrade); 
                        std::cout << "Upgraded tower with " << upgrade->name << std::endl;
                    }
                    closeTowerUI();
                } else {
                    std::cout << "Not enough money!" << std::endl;
                }
            });


            active_ui_elements_.push_back(button);
            menu_buttons_.push_back(button);
            addUISprite(button);
            startY += buttonHeight + stepY;
        }
    }

    // We then add a sell button
    int id = tower->getId();
    int cost = tower->getCurrentUpgradeNode()->cost;

    auto buttonSell = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, 50.0f);
    buttonSell->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, 25.0f, 0.0f}, buttonWidth, 50.0f, {150, 80, 80, 255})); // Distinct reddish color


    auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, "SELL", Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
    buttonSell->addSubSprite(text);
    
    buttonSell->setOnLeftClick([this,id,cost](){
        std::lock_guard<std::recursive_mutex> lock(render_mutex_);
        // We unselect the tower from the user
        auto to_delete = selected_tower_.lock();
        selected_tower_.reset();
        if (to_delete) {
            // Check if the tower was on an augment case
            Point tower_cell{std::floor(to_delete->getPosition().getX()), std::floor(to_delete->getPosition().getY())};
            auto it_tac = tac_towers.find(tower_cell);
            if (it_tac != tac_towers.end()) {
                // Remove the augment from all affected towers
                for (auto& wp : it_tac->second) {
                    if (auto affected_tower = wp.lock()) {
                        affected_tower->removeAugment("Slowness");
                    }
                }
                // Reset the augmented list
                it_tac->second.clear();
            }

            // Remove the tower from any other augment lists to avoid dangling pointers
            for (auto& pair : tac_towers) {
                auto& affected_towers = pair.second;
                affected_towers.erase(
                    std::remove_if(affected_towers.begin(), affected_towers.end(), [&to_delete](const std::weak_ptr<Tower>& wp) {
                        auto pt = wp.lock();
                        return !pt || pt == to_delete;
                    }),
                    affected_towers.end()
                );
            }

            removeEntity(to_delete.get());
        }
        
        
        money_+=(cost/2);

        // We remove it from the tower list if it still exists
        // first we find it
        auto it = std::find_if(
            placed_towers_.begin(),
            placed_towers_.end(),
            [id](const std::shared_ptr<Tower>& t) {
                return t->getId() == id;
            }
        );
        // We use the mutex to correctly remove it when no other thread use it.
        if (it != placed_towers_.end()) {
            sold_towers_.push_back(*it);
            placed_towers_.erase(it);
        }
        closeTowerUI();
    });
    
    active_ui_elements_.push_back(buttonSell);
    menu_buttons_.push_back(buttonSell);
    addUISprite(buttonSell);
    
    startY += 50.0f + 10.0f;

    ui_panel_h_ = (startY - ui_panel_y_) + 120.0f;

    auto bg = Sprites::rectangle({ui_panel_x_ + ui_panel_w_ / 2.0f, ui_panel_y_ + ui_panel_h_ / 2.0f, -10.0f}, ui_panel_w_, ui_panel_h_, {20, 20, 40, 200});
    active_ui_elements_.push_back(bg);
    addUISprite(bg);
}

void UI::Session::drawUI(SDL_Renderer* r) {
    // Draw selection highlight on the currently selected menu button
    if (!menu_buttons_.empty() && static_cast<size_t>(menu_button_index_) < menu_buttons_.size()) {
        auto selected_button = menu_buttons_[menu_button_index_];
        
        // Draw a bright border around the selected button
        auto pos = selected_button->getPosition();
        SDL_Color col = {255, 200, 0, 255};
        float btn_w = selected_button->getWidth();
        float btn_h = selected_button->getHeight();
        
        float btnOffsetX = (pos.getX() < 0) ? getWinWidth() : 0.0f;
        float btnOffsetY = (pos.getY() < 0) ? getWinHeight() : 0.0f;
        
        Session::drawHighlightBox(r, delta_time_, Point{btnOffsetX, btnOffsetY}, ui_scale_, pos.getX() - 4.0f, pos.getY() - 4.0f, btn_w + 8.0f, btn_h + 8.0f, 2.0f, col);
    }

    if (!showUI_) return;
    auto st = selected_tower_.lock();
    if (showUI_ && st) {

        int lvl = st->getLevel();

        // The tower badge is colored by level
        SDL_Color badgeColor;
        if (lvl == 1)      badgeColor = SDL_Color{180, 180, 180, 255};
        else if (lvl == 2) badgeColor = SDL_Color{100, 220, 100, 255};
        else if (lvl == 3) badgeColor = SDL_Color{100, 150, 255, 255};
        else if (lvl == 4) badgeColor = SDL_Color{180, 100, 255, 255};
        else               badgeColor = SDL_Color{255, 215, 0, 255};

        // UI settings
        float titleX = ui_panel_x_ + 10.0f;
        float titleY = ui_panel_y_ + 10.0f;
        float badgeSize = 26.0f;
        float badgeX = titleX + 200.0f;
        float badgeY = titleY + 4.0f;
        float badgeOffsetX = (badgeX < 0) ? getWinWidth() : 0.0f;
        float badgeOffsetY = (badgeY < 0) ? getWinHeight() : 0.0f;
        Point badge_offset{badgeOffsetX, badgeOffsetY};

        auto badge = Sprites::rectangle({badgeX + badgeSize / 2.0f, badgeY + badgeSize / 2.0f, 0.0f}, badgeSize, badgeSize, badgeColor);
        badge->draw(r, delta_time_, badge_offset, ui_scale_, 0.0f);

        Sprites::Text lvlText(
            {badgeX + badgeSize / 2.0f, badgeY + badgeSize / 2.0f, 12.0f},
            std::to_string(lvl),
            Sprites::Text::POKETEXT,
            20,
            SDL_Color{0,0,0,255},
            true
        );
        lvlText.draw(r, delta_time_, badge_offset, ui_scale_, 0.0f);
    }

    // XP BAR
    if (showUI_ && st) {

        int xp = st->getXp();
        int xpMax = st->getXpMax();
        int level = st->getLevel();
        int levelMax = st->getLevelMax();
        float xpRatio = (level >= levelMax) ? 1.0f : std::min(1.0f, xp / (float)xpMax);

        float margin = 20.0f;
        float barX = ui_panel_x_ + margin;
        float barY = ui_panel_y_ + ui_panel_h_ - 80.0f; // position basse
        float barW = ui_panel_w_ - 2 * margin;
        float barH = 25.0f;
        
        float barOffsetX = (barX < 0) ? getWinWidth() : 0.0f;
        float barOffsetY = (barY < 0) ? getWinHeight() : 0.0f;
        Point bar_offset{barOffsetX, barOffsetY};

        auto bg = Sprites::rectangle({barX + barW / 2.0f, barY + barH / 2.0f, 0.0f}, barW, barH, {80, 80, 80, 255});
        bg->draw(r, delta_time_, bar_offset, ui_scale_, 0.0f);

        if (xpRatio > 0.0f) {
            float fillW = barW * xpRatio;
            auto fill = Sprites::rectangle({barX + fillW / 2.0f, barY + barH / 2.0f, 0.0f}, fillW, barH, {100, 180, 255, 255});
            fill->draw(r, delta_time_, bar_offset, ui_scale_, 0.0f);
        }

        if (level >= levelMax) {
            Sprites::Text xpValue(
                {barX + barW / 2.0f, barY + barH / 2.0f, 12.0f},
                "MAX",
                Sprites::Text::POKETEXT,
                20,
                SDL_Color{255, 215, 0, 255},
                true
            );
            xpValue.draw(r, delta_time_, bar_offset, ui_scale_, 0.0f);
        }
        else {
            Sprites::Text xpValue(
                {barX, barY - 28.0f, 12.0f},
                "XP: " + std::to_string(xp) + " / " + std::to_string(xpMax),
                Sprites::Text::POKETEXT,
                18,
                SDL_Color{255,255,255,255}
            );
            xpValue.draw(r, delta_time_, bar_offset, ui_scale_, 0.0f);
        }
    }
}

void UI::Session::drawSelection(SDL_Renderer* r) {
    if (!selected_cell_ || hp_player_ <= 0) return;
    Point c = *selected_cell_;
    float x = c.getX();
    float y = c.getY();
    SDL_Color col = {255, 255, 0, 255}; // jaune
    float thickness = 0.05f; // épaisseur du cadre
    Session::drawHighlightBox(r, delta_time_, camera_position_, scale_, x, y, 1.0f, 1.0f, thickness, col);
}



void UI::Session::drawHighlightBox(SDL_Renderer* r, float dt, Point offset, float scale, float x, float y, float w, float h, float thickness, SDL_Color col) {
    auto top = Sprites::rectangle({x + w / 2.0f, y + thickness / 2.0f, 999.0f}, w, thickness, col);
    auto bottom = Sprites::rectangle({x + w / 2.0f, y + h - thickness / 2.0f, 999.0f}, w, thickness, col);
    auto left = Sprites::rectangle({x + thickness / 2.0f, y + h / 2.0f, 999.0f}, thickness, h, col);
    auto right = Sprites::rectangle({x + w - thickness / 2.0f, y + h / 2.0f, 999.0f}, thickness, h, col);

    top->draw(r, dt, offset, scale, 0.0f);
    bottom->draw(r, dt, offset, scale, 0.0f);
    left->draw(r, dt, offset, scale, 0.0f);
    right->draw(r, dt, offset, scale, 0.0f);
}


// ------------------------------------------------
//                  GAMEPLAY LOOP 
// ------------------------------------------------

void UI::Session::startNextWave() {
    if (!waveActive_) {
        round_++;
        wave_spawns_.clear();
        // If we have wave config we use it
        if (!wave_configs_.empty()) {
            // We iterate through every type of the config to prepare each enemy.
            for (const auto& config : wave_configs_) {
                // We use a base amount, a linear scale and a exponential for late game
                int quantity = config.baseQuantity;
                quantity += static_cast<int>(round_ * config.linearScaler);
                quantity += static_cast<int>(std::pow(round_, config.exponentialScaler));
                
                std::weak_ptr<EnemyBlueprint> bp;
                // We search the blueprint corresponding to the type
                // if it's not found we just skip this type
                for (const auto& cat : enemy_catalog_) {
                    if (cat->getType() == config.enemyType) {
                        bp = cat;
                        break;
                    }
                }
                if (!bp.expired())
                for (int i = 0; i < quantity; ++i) {
                    wave_spawns_.push_back(bp);
                }
            }
            // The wave is packed by type, we shuffle the list to get a mixed wave.
            // Simple Fisher-Yates shuffle :: https://fr.wikipedia.org/wiki/M%C3%A9lange_de_Fisher-Yates
            for (int i = static_cast<int>(wave_spawns_.size()) - 1; i > 0; --i) {
                int j = rand() % (i + 1);
                std::swap(wave_spawns_[i], wave_spawns_[j]);
            }
            
            enemiesToSpawn_ = wave_spawns_.size();
        } else { // just set a fix amount to spawn if no wave config given
            enemiesToSpawn_ = 5 + round_ * 6; 
        }
        
        spawnTimer_ = 0.0f;
        waveActive_ = true;
        std::cout << "Wave " << round_ << " starting! Enemies: " << enemiesToSpawn_ << "\n";
    }
}

void UI::Session::spawnEnemy(float cellSize, Point spawningDirection, float baseX, float baseY, std::list<Point>& path, std::vector<std::unique_ptr<Enemy>>& el) {
    // Spread the enemies across 90% of the cell width so they are visibly spaced out
    float offsetSpawn = (rand() / (float)RAND_MAX - 0.5f) * cellSize * 0.9f;
    Point spawnOffset = Point{-spawningDirection.getY(), spawningDirection.getX()} * offsetSpawn;
    Point spawnPosition{baseX, baseY};
    spawnPosition += spawnOffset;
    
    std::shared_ptr<EnemyBlueprint> blueprint = nullptr;

    // We check for elements in the wavespawn config array, if there's none means we're using the old way
    if (!wave_spawns_.empty()) {
        // we need to swap ownership of the vector for memory safe action
        blueprint = wave_spawns_.back().lock();
        wave_spawns_.pop_back();
    } else {

        // We use a 1/3 of the enemy to be flying.
        bool is_flying = (enemiesToSpawn_ % 3 == 0);

        // We iterate through the catalog to find flying/ground accordingly
        std::vector<std::shared_ptr<EnemyBlueprint>> matching_blueprints;
        for(const auto& bp : enemy_catalog_) {
            if (bp->isFlying() == is_flying) {
                matching_blueprints.push_back(bp);
            }
        }

        if (matching_blueprints.empty()) return; // Should not happen if blueprints are loaded

        // We then take a random one to be spawn
        int randomIndex = rand() % matching_blueprints.size();
        blueprint = matching_blueprints[randomIndex];
    }
    
    if (!blueprint) return;
    
    // Visually scale the enemy to fit within a tile
    float enemySize = cellSize * 0.095f;
    el.push_back(blueprint->instantiateEnemy(spawnPosition, offsetSpawn, path.begin(), path.end(), enemySize, round_));
    addEntity(el.back().get());
    enemiesToSpawn_--;
    spawnTimer_ = 0.0f;
}

void UI::Session::GameOverScreen(){
    if(showUI_) closeTowerUI();
    // We clear everything first
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
    sprites_.clear();
    entities_.clear();
    ui_sprites_.clear();
    active_ui_elements_.clear();
    menu_buttons_.clear();
    showUI_ = false;

    // We prepare the title gameover in the center
    float cx = getWinWidth() / 2.0f;
    float cy = getWinHeight() / 2.0f;

    auto go_title = std::make_shared<Sprites::Text>(std::array<float, 3>{cx, cy - 100.0f, 11.0f}, "GAME OVER", Sprites::Text::POKETEXT, 48, SDL_Color{255, 50, 50, 255}, true);
    
    auto go_btn = std::make_shared<Sprites::Button>(std::array<float, 3>{cx - 100.0f, cy, 10.0f}, 200.0f, 60.0f);
    go_btn->addSubSprite(Sprites::rectangle({100.0f, 30.0f, 0.0f}, 200.0f, 60.0f, {150, 50, 50, 255}));
    auto btn_text = std::make_shared<Sprites::Text>(std::array<float, 3>{100.0f, 30.0f, 1.0f}, "QUIT", Sprites::Text::POKETEXT, 24, SDL_Color{255, 255, 255, 255}, true);
    go_btn->addSubSprite(btn_text);

    // The button generate a quit event
    go_btn->setOnLeftClick([this]() {
        wants_to_die_ = true;
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    });
    
    active_ui_elements_.push_back(go_title);
    active_ui_elements_.push_back(go_btn);
    addUISprite(go_title);
    addUISprite(go_btn);
    menu_button_index_ = 0;
    menu_buttons_.push_back(go_btn);
}

void UI::Session::checkAddAugmentedCellBonus(){
    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
    // We check every augmented cells in case the build affected
    for (auto& augCell : tower_augment_cells) {

        float augX = augCell.getX();
        float augY = augCell.getY();

        // Check if there is actually a tower built on this augment cell
        bool has_tower = false;
        for (auto& t : placed_towers_) {
            if (std::floor(t->getPosition().getX()) == augX &&
                std::floor(t->getPosition().getY()) == augY) {
                has_tower = true;
                break;
            }
        }
        
        if (!has_tower) continue;
        // If there is a tower we check every tower around to apply the effect
        for (auto& tower : placed_towers_) {

            float tx = tower->getPosition().getX();
            float ty = tower->getPosition().getY();

            float dx = tx - augX;
            float dy = ty - augY;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist < auraRadius) {
                auto it = tac_towers.find(augCell);
                if (it != tac_towers.end()) {
                    auto& affected_towers = it->second;
                    auto found = std::find_if(affected_towers.begin(), affected_towers.end(), [&tower](const std::weak_ptr<Tower>& wp){ return wp.lock() == tower; });
                    if (found == affected_towers.end()) {
                        tower->addAugment(std::make_unique<SlownessAugment>(0.5f));
                        affected_towers.push_back(tower);
                    }
                }
            }
        }
    }

}

void UI::Session::mainSession() {
    while(!Window::sdl_initiated) {
        std::this_thread::yield(); // wait for sdl to be ready
    }

    // we scale the game map based on the window and the map size.
    float cellWidth  = (getWinWidth()-300) / static_cast<float>(map_.getWidth());
    float cellHeight = getWinHeight() / static_cast<float>(map_.getHeight());
    scale_ = std::min(cellWidth, cellHeight);
    auraRadius = 3.0f; // Cell augment radius effect

    // Center the map on the screen
    float offsetX = ((getWinWidth()-300)  - scale_ * map_.getWidth())  / 2.0f;
    float offsetY = (getWinHeight() - scale_ * map_.getHeight()) / 2.0f;
    camera_position_ = Point{offsetX, offsetY};


    // Player stat on the UI
    auto moneyText = std::make_shared<Sprites::Text>(std::array<float, 3>{20.0f, 20.0f, 10.0f}, "Money: " + std::to_string(money_) + "$", Sprites::Text::POKETEXT, 24, SDL_Color{255, 215, 0, 255});
    addUISprite(moneyText);
    
    auto hpText = std::make_shared<Sprites::Text>(std::array<float, 3>{getWinWidth() / 2.0f, -40.0f, 10.0f}, "HP: " + std::to_string(hp_player_), Sprites::Text::POKETEXT, 24, SDL_Color{255, 50, 50, 255}, true);
    addUISprite(hpText);

    // Prepare to read stats changes that need to trigger update to the text
    int last_money = money_;
    int last_hp = hp_player_;

    // Start wave button
    auto bouton_next_wave = std::make_shared<Sprites::Button>(std::array<float, 3>{60.0f, -60, 10.0f}, 260.0f, 50.0f);
    auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, "START WAVE", Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
    bouton_next_wave->addSubSprite(text);

    bouton_next_wave->setOnLeftClick([this]() {
        startNextWave();
    });
    addUISprite(bouton_next_wave);

    generateMapSprites();

     // Setup the path for enemies
    std::list<Point> path = map_.getPath();

    for(auto& p : path) {
        float px = p.getY() * cellSize + cellSize / 2.0f;
        float py = p.getX() * cellSize + cellSize / 2.0f;
        p = Point{px, py};
    }

    float baseX = path.front().getX();
    float baseY = path.front().getY();


    std::vector<std::unique_ptr<Enemy>> el = {};
    // operator overload to setup path direction we use \^ to say we point from A to B (for fun)
    Point spawningDirection = ((*path.begin())^(*(++path.begin())));

    using clock = std::chrono::steady_clock;
    auto lastTime = clock::now();
    
    bool running = true;
    while(running && !wants_to_die_) { // Game loop
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        if (money_ != last_money) {
            std::lock_guard<std::recursive_mutex> lock(render_mutex_);
            moneyText->setText("Money: " + std::to_string(money_) + "$");
            last_money = money_;
        }
        if (hp_player_ != last_hp) {
            std::lock_guard<std::recursive_mutex> lock(render_mutex_);
            hpText->setText("HP: " + std::to_string(hp_player_));
            last_hp = hp_player_;
        }
        if (waveActive_) {
            spawnTimer_ += dt;
            
            // Spawn enemies if we still have some left to spawn for this wave
            float rtime = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (enemiesToSpawn_ > 0 && spawnTimer_ >= (0.01f + 0.2*rtime*rtime) / (round_+1)) {
                spawnEnemy(cellSize, spawningDirection, baseX, baseY, path, el);
            }

            // Update existing enemies
            for (auto& enemy : el) {
                if (!enemy->isAlive()) continue;
                enemy->live(dt);
                
                if (enemy->hasReachedEnd()) {
                    hpSetter(hp_player_ - 1);
                    std::cout << "Player took damage! HP: " << hp_player_ << "\n";
                    enemy->kill();
                    removeEntity(enemy.get());
                } else if (enemy->getLp() <= 0) {
                    moneySetter(money_ + 10);
                    enemy->kill();
                    removeEntity(enemy.get());
                }
            }
            
            map_ope_.clear(); // Clear our quadtree and create a new set
            for (auto& enemy : el) {
                if (enemy->isAlive()) map_ope_.addEnemy(enemy.get());
            }
             // Update projectiles
            for (auto it = active_projectiles_.begin(); it != active_projectiles_.end(); ) {
                auto& proj = *it;
                proj->live(dt);
                
                if (proj->hasHit()) { // If projectile hit it's target we actualize the list of affected entity with quadtree
                    std::vector<Enemy*> hit_enemies;
                    if (proj->getSize() > 0.0f) { // Splash damage
                        std::vector<Enemy*> nearby = map_ope_.query(proj->getPosition(), proj->getSize());
                        for (auto* enemy : nearby) {
                            hit_enemies.push_back(enemy);
                        }
                    } else { // Single target
                        Enemy* closest = nullptr;
                        float min_dist = 1.0f; // Max acceptable dist for single target splash search
                        std::vector<Enemy*> nearby = map_ope_.query(proj->getPosition(), min_dist);
                        for (auto* enemy : nearby) {
                            Point dir = proj->getPosition() ^ enemy->getPosition();
                            float dist = std::sqrt(dir.getX()*dir.getX() + dir.getY()*dir.getY());
                            if (dist <= min_dist) {
                                min_dist = dist;
                                closest = enemy;
                            }
                        }
                        if (closest) hit_enemies.push_back(closest);
                    }
                    
                    proj->hit(hit_enemies);
                    
                    removeEntity(proj.get());
                    it = active_projectiles_.erase(it);
                } else {
                    ++it;
                }
            }
            // Update towers with enemy list and the quadtree
            std::vector<std::weak_ptr<Tower>> current_towers;
            {
                std::lock_guard<std::recursive_mutex> lock(render_mutex_);
                for (auto& tower : placed_towers_) {
                    current_towers.push_back(tower);
                }
            }
            
            std::vector<std::unique_ptr<Projectile>> all_new_projs;
            for (auto& weak_tower : current_towers) {
                if (auto tower = weak_tower.lock()) {
                    std::vector<Enemy*> nearby_enemies = map_ope_.allWithinRange(*tower);
                    tower->live(dt, nearby_enemies);
                    auto new_projs = tower->fetchSpawnedProjectiles();
                    for(auto& p : new_projs) {
                        all_new_projs.push_back(std::move(p));
                    }
                }
            }

            for(auto& p : all_new_projs) {
                addEntity(p.get());
                active_projectiles_.push_back(std::move(p));
            }


            
            // Check if wave is over (no more to spawn and all enemies are dead)
            bool allEnemiesDead = true;
            for (auto& enemy : el) {
                if (enemy->isAlive()) {
                    allEnemiesDead = false;
                    break;
                }
            }
            
            if (enemiesToSpawn_ <= 0 && allEnemiesDead) {
                // Clean up all dead enemies at wave end
                for (auto& enemy : el) {
                    removeEntity(enemy.get());
                }
                el.clear();
                
                // Clean up remaining projectiles from the wave
                for (auto& proj : active_projectiles_) {
                    removeEntity(proj.get());
                }
                active_projectiles_.clear();
                
                {
                    std::lock_guard<std::recursive_mutex> lock(render_mutex_);
                    sold_towers_.clear();
                
                    for (auto& tower : placed_towers_) {
                        tower->clearDetachedAugments();
                    }
                }
                
                waveActive_ = false;
                std::cout << "Wave " << round_ << " clear! Waiting for next wave...\n";
            }
        }
        
        // If the player's dead we set the gameover screen
        if (hp_player_ <= 0) {
            std::cout << "GAME OVER!\n";
            GameOverScreen();

            while (!wants_to_die_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            running = false;
        }

        auto loop_end = clock::now();
        float loop_time = std::chrono::duration<float>(loop_end - now).count();
        int sleep_time = 16 - static_cast<int>(loop_time * 1000.0f);
        if (sleep_time < 4) sleep_time = 4; // Always yield at least 4ms to the UI thread
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }

    // We clean the memory.
    for (auto& enemy : el) {
        removeEntity(enemy.get());
    }
    {
        std::lock_guard<std::recursive_mutex> lock(render_mutex_);
        for (auto& tower : placed_towers_) {
            removeEntity(tower.get());
        }
    }
    for (auto& proj : active_projectiles_) {
        removeEntity(proj.get());
    }
    {
        std::lock_guard<std::recursive_mutex> lock(render_mutex_);
        sold_towers_.clear();
        placed_towers_.clear();
    }
}
