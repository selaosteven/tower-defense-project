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
    selected_tower_{nullptr}
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
    }

void UI::Session::startNextWave() {
    if (!waveActive_) {
        round_++;
        enemiesToSpawn_ = 5 + round_ * 2; // Increase difficulty: 7, 9, 11 enemies...
        spawnTimer_ = 0.0f;
        waveActive_ = true;
        std::cout << "Wave " << round_ << " starting! Enemies: " << enemiesToSpawn_ << "\n";
    }
}

void UI::Session::moneySetter(int new_money) {
    money_ = new_money;
}

void UI::Session::hpSetter(int new_hp) {
    hp_player_ = new_hp;
}

void UI::Session::closeTowerUI() {
    // With std::shared_ptr here and std::weak_ptr in Window, clearing this vector
    // automatically triggers cleanup from Window's ui_sprites_ list!
    active_ui_elements_.clear();
    menu_buttons_.clear();
    menu_button_index_ = 0;
    if (selected_tower_) {
        selected_tower_->setShowRange(false);
    }
    showUI_ = false;
    selected_tower_ = nullptr;
    selected_cell_.reset();
}

void UI::Session::openBuildUI(Point cell) {
    if (showUI_) closeTowerUI(); // Close any existing UI first

    showUI_ = true;
    selected_cell_ = cell;
    menu_button_index_ = 0;
    menu_buttons_.clear();
    
    float startY = ui_panel_y_ + 10.0f;
    float stepY = 60.0f;
    float margin = 20.0f;
    float buttonWidth = ui_panel_w_ - 2 * margin;

    // Title
    auto title = std::make_shared<Sprites::Text>(std::array<float, 3>{ui_panel_x_ + 10.0f, startY, 11.0f}, "Build Tower", Sprites::Text::POKETEXT, 24, SDL_Color{255, 255, 255, 255});
    active_ui_elements_.push_back(title);
    addUISprite(title);
    startY += 40;

    for (const auto& blueprint : tower_catalog_) {
        int cost = blueprint->getRootUpgrade() ? blueprint->getRootUpgrade()->cost : 0;
        std::string label = blueprint->getTowerType() + " (" + std::to_string(cost) + "$)";

        auto button = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, 50.0f);
        button->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, 25.0f, 0.0f}, buttonWidth, 50.0f, {80, 80, 150, 255}));
        
        auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, label, Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
        button->addSubSprite(text);

        button->setOnLeftClick([this, blueprint = blueprint.get(), cost, cell = *selected_cell_]() {
            if (money_ >= cost) {
                money_ -= cost;
                
                float logicX = cell.getX() + 0.5f;
                float logicY = cell.getY() + 0.5f;

                Projectile dummyProj(-0.1f, 5.0f); 
                auto new_tower = blueprint->instantiateTower({logicX, logicY}, dummyProj);
                
                addEntity(new_tower.get());
                placed_towers_.push_back(std::move(new_tower));

                std::cout << "Built a " << blueprint->getTowerType() << " at " << logicX << ", " << logicY << std::endl;
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

}

void UI::Session::openUpgradeUI(Tower* tower) {
    if (showUI_) closeTowerUI();

    // Show tower range
    showUI_ = true;
    selected_tower_ = tower;
    selected_tower_->setShowRange(true);
    menu_button_index_ = 0;
    menu_buttons_.clear();

    float startY = ui_panel_y_ + 10.0f;
    float stepY = 20.0f;
    float UI_WIDTH = 300.0f; //, 400.0f
    float margin = 20.0f;
    float buttonWidth = ui_panel_w_ - 2 * margin;
    int max_width_text = buttonWidth - 30; // padding of 15px per side

    auto title = std::make_shared<Sprites::Text>(std::array<float, 3>{ui_panel_x_ + 10.0f, startY, 11.0f}, tower->getType(), Sprites::Text::POKETEXT, 24, SDL_Color{255, 255, 255, 255});
    active_ui_elements_.push_back(title);
    addUISprite(title);
    startY += 40;

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
    const UpgradeNode* current_node = tower->getCurrentUpgradeNode();
    if (!current_node || current_node->children.empty()) {
        auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{ui_panel_x_ + margin, startY, 1.0f}, "No upgrades available.", Sprites::Text::POKETEXT, 14, SDL_Color{255, 255, 255, 255});
        active_ui_elements_.push_back(text);
        addUISprite(text);
        startY += 40.0f;
    } 
    else {
        for (const auto& upgrade_node_ptr : current_node->children) {
            const UpgradeNode* upgrade = upgrade_node_ptr.get();
            std::string label = upgrade->name + " (" + std::to_string(static_cast<int>(upgrade->cost)) + "$)";
            auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, label, Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255}, max_width_text);
            float buttonHeight = std::max(50.0f, static_cast<float>(text->getHeight() + 30.0f));
            auto button = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, buttonHeight);
            button->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, buttonHeight / 2.0f, 0.0f}, buttonWidth, buttonHeight, {80, 80, 150, 255}));
            button->addSubSprite(text);
            button->setOnLeftClick([this, upgrade]() { // Pour faire un upgrade
                if (money_ >= upgrade->cost) {
                    money_ -= upgrade->cost;
                    selected_tower_->applyUpgrade(upgrade); 
                    std::cout << "Upgraded tower with " << upgrade->name << std::endl;
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

    // Création du boutton de vente

    // Récupération des infos de la tour sélectionnée
    int id = tower->getId();
    int cost = tower->getCurrentUpgradeNode()->cost;

    auto buttonSell = std::make_shared<Sprites::Button>(std::array<float, 3>{ui_panel_x_ + margin, startY, 10.0f}, buttonWidth, 50.0f);
    buttonSell->addSubSprite(Sprites::rectangle({buttonWidth / 2.0f, 25.0f, 0.0f}, buttonWidth, 50.0f, {150, 80, 80, 255})); // Distinct reddish color


    auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, "SELL", Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
    buttonSell->addSubSprite(text);
    
    buttonSell->setOnLeftClick([this,id,cost](){
        
        // 1) Sauvegarder la tour sélectionnée
        Tower* to_delete = selected_tower_;

        // 2) Couper le pointeur AVANT destruction
        selected_tower_ = nullptr;

        // 3) Retirer la tour du moteur
        if (to_delete) {
            removeEntity(to_delete);
        }
        
        
        money_+=(cost/2);

        // 3) La retirer de la liste des tours en différant la destruction
        auto it = std::find_if(
            placed_towers_.begin(),
            placed_towers_.end(),
            [id](const std::unique_ptr<Tower>& t) {
                return t->getId() == id;
            }
        );
        if (it != placed_towers_.end()) {
            std::thread([t = std::move(*it)]() mutable {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                // La tour est détruite en toute sécurité ici quand `t` sort de la portée
            }).detach();
            placed_towers_.erase(it);
        }
        closeTowerUI();
    });
    
    active_ui_elements_.push_back(buttonSell);
    menu_buttons_.push_back(buttonSell);
    addUISprite(buttonSell);
    
    startY += 50.0f + 10.0f;

    ui_panel_h_ = (startY - ui_panel_y_) + 120.0f;
}

void UI::Session::clickLeft(Point click) {
    // If the UI is open, clicks on UI buttons are handled by the buttons themselves.
    // We only need to check for clicks *outside* the UI panel to close it.
    if (showUI_) {
        SDL_Rect uiRect = {
            static_cast<int>(ui_panel_x_ * ui_scale_),
            static_cast<int>(ui_panel_y_ * ui_scale_),
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
    
    // --- UI is not open, handle world clicks ---

    float clickLX = (click.getX() - camera_position_.getX()) / scale_;
    float clickLY = (click.getY() - camera_position_.getY()) / scale_;

    int cellX = static_cast<int>(std::floor(clickLX));
    int cellY = static_cast<int>(std::floor(clickLY));

    if (cellY < 0 || cellY >= map_.getHeight() || cellX < 0 || cellX >= map_.getWidth() || map_.map_[cellY][cellX] != Case::Tower) {
        return;
    }

    Tower* clicked_tower = nullptr;
    for (auto const& tower : placed_towers_) {
        Point tower_pos = tower->getPosition();
        if (static_cast<int>(std::floor(tower_pos.getX())) == cellX && static_cast<int>(std::floor(tower_pos.getY())) == cellY) {
            clicked_tower = tower.get();
            break;
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
    if (showUI_ && !menu_buttons_.empty()) {
        menu_buttons_[menu_button_index_]->triggerLeftClick();
        return;
    }
    
    if (!selected_cell_) return;

    float screenX = selected_cell_->getX() * scale_ + camera_position_.getX() + scale_ * 0.5f;
    float screenY = selected_cell_->getY() * scale_ + camera_position_.getY() + scale_ * 0.5f;

    clickLeft(Point{screenX, screenY});
}

void UI::Session::onArrowUp() {
    if (!showUI_ || menu_buttons_.empty()) return;
    
    menu_button_index_ = (menu_button_index_ - 1 + menu_buttons_.size()) % menu_buttons_.size();
}

void UI::Session::onArrowDown() {
    if (!showUI_ || menu_buttons_.empty()) return;
    
    menu_button_index_ = (menu_button_index_ + 1) % menu_buttons_.size();
}

void UI::Session::onEscape(){
    if(showUI_) closeTowerUI();
}

void UI::Session::onSpace(){
    startNextWave();
}

void UI::Session::drawUI(SDL_Renderer* r) {
    if (!showUI_) return;    
    
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 20, 20, 40, 200);
    SDL_FRect bgRect = {ui_panel_x_ * ui_scale_, ui_panel_y_ * ui_scale_, ui_panel_w_ * ui_scale_, ui_panel_h_ * ui_scale_};
    SDL_RenderFillRectF(r, &bgRect);
    
    // Draw selection highlight on the currently selected menu button
    if (!menu_buttons_.empty() && static_cast<size_t>(menu_button_index_) < menu_buttons_.size()) {
        auto selected_button = menu_buttons_[menu_button_index_];
        
        // Draw a bright border around the selected button
        auto pos = selected_button->getPosition();
        SDL_Color col = {255, 200, 0, 255}; // Gold color
        // The width and height are mapped from the original dimensions, with added spacing so the highlight is visibly framing the button boundaries 
        float btn_w = selected_button->getWidth();
        float btn_h = selected_button->getHeight();
        Session::drawHighlightBox(r, delta_time_, Point{0.0f, 0.0f}, ui_scale_, pos.getX() - 4.0f, pos.getY() - 4.0f, btn_w + 8.0f, btn_h + 8.0f, 2.0f, col);
    }

    // --- BARRE D'XP DYNAMIQUE ---
    if (showUI_ && selected_tower_) {

        int xp = selected_tower_->getXp();
        int xpMax = 100;
        float xpRatio = std::min(1.0f, xp / (float)xpMax);

        float margin = 20.0f;
        float barX = ui_panel_x_ + margin;
        float barY = ui_panel_y_ + ui_panel_h_ - 80.0f; // position basse
        float barW = ui_panel_w_ - 2 * margin;
        float barH = 25.0f;

        // Fond gris
        SDL_FRect bg = { barX * ui_scale_, barY * ui_scale_, barW * ui_scale_, barH * ui_scale_ };
        SDL_SetRenderDrawColor(r, 80, 80, 80, 255);
        SDL_RenderFillRectF(r, &bg);

        // Barre bleue
        SDL_FRect fill = { barX * ui_scale_, barY * ui_scale_, (barW * xpRatio) * ui_scale_, barH * ui_scale_ };
        SDL_SetRenderDrawColor(r, 100, 180, 255, 255);
        SDL_RenderFillRectF(r, &fill);

        // --- TEXTE DYNAMIQUE "XP: x / y" ---
        {
            Sprites::Text xpValue(
                {barX, barY - 20.0f, 12.0f},
                "XP: " + std::to_string(xp) + " / " + std::to_string(xpMax),
                Sprites::Text::POKETEXT,
                18,
                SDL_Color{255,255,255,255}
            );
            xpValue.draw(r, delta_time_, Point{0,0}, ui_scale_, 0.0f);
        }
    }

}

void UI::Session::drawSelection(SDL_Renderer* r) {
    if (!selected_cell_) return;

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


void UI::Session::mainSession() {
    while(!Window::sdl_initiated);
    float cellWidth  = (getWinWidth()-300) / static_cast<float>(map_.getWidth());
    float cellHeight = getWinHeight() / static_cast<float>(map_.getHeight());
    scale_ = std::min(cellWidth, cellHeight);
    float cellSize = 1.0f;

    // Center the map on the screen for the rendering engine
    float offsetX = ((getWinWidth()-300)  - scale_ * map_.getWidth())  / 2.0f;
    float offsetY = (getWinHeight() - scale_ * map_.getHeight()) / 2.0f;
    camera_position_ = Point{offsetX, offsetY};


    // Status UI Elements
    auto moneyText = std::make_shared<Sprites::Text>(std::array<float, 3>{20.0f, 20.0f, 10.0f}, "Money: " + std::to_string(money_) + "$", Sprites::Text::POKETEXT, 24, SDL_Color{255, 215, 0, 255});
    addUISprite(moneyText);
    
    auto hpText = std::make_shared<Sprites::Text>(std::array<float, 3>{getWinWidth() / 2.0f, -40.0f, 10.0f}, "HP: " + std::to_string(hp_player_), Sprites::Text::POKETEXT, 24, SDL_Color{255, 50, 50, 255}, true);
    addUISprite(hpText);

    // START WAVE BUTTON
    auto bouton_next_wave = std::make_shared<Sprites::Button>(std::array<float, 3>{60.0f, -60, 10.0f}, 260.0f, 50.0f);
    auto text = std::make_shared<Sprites::Text>(std::array<float, 3>{15.0f, 15.0f, 1.0f}, "START WAVE", Sprites::Text::POKETEXT, 18, SDL_Color{255, 255, 255, 255});
    bouton_next_wave->addSubSprite(text);

    bouton_next_wave->setOnLeftClick([this]() {
        startNextWave();
    });
    addUISprite(bouton_next_wave);
    int last_money = money_;
    int last_hp = hp_player_;

    for(int y = 0; y < map_.getHeight(); y++) {
        for(int x = 0; x < map_.getWidth(); x++) {
            
            Case bloc = map_.map_.at(y).at(x);
            if(bloc == Case::Tower){
                tower_build_cells_.push_back(Point{(float)x, (float)y});
            }
            std::shared_ptr<Sprites::Sprite> s = nullptr; // On prépare un pointeur vide

            float px = x * cellSize + cellSize / 2.0f;
            float py = y * cellSize + cellSize / 2.0f;

            switch (bloc) {
                case Case::Tower:
                    s = Sprites::circle({px, py, 99}, cellSize/4); // circle already returns std::shared_ptr
                    break;
                case Case::Path:
                    s = Sprites::rectangle({px, py, 99}, cellSize/3);
                    break;
                case Case::Wall:
                    break;
                case Case::Start:
                    s = Sprites::triangle({px, py, 99}, cellSize/4);
                    break;
                case Case::End:
                    s = Sprites::triangle({px, py, 99}, cellSize/4);
                    break;
                
                default:
                    s = Sprites::rectangle({px, py, 99}, cellSize/8,cellSize/8,(SDL_Color){125,80,125,200});
                    break; // On passe au suivant si c'est du vide
            }

            if (s) {                                
                // 5. ENVOI AU MOTEUR : Ton addSprite reçoit un pointeur VALIDE
                addSprite(s); 
            }
        }
    }

     // Spawn des enemies
    std::list<Point> path = map_.getPath();

    for(auto& p : path) {
        float px = p.getY() * cellSize + cellSize / 2.0f;
        float py = p.getX() * cellSize + cellSize / 2.0f;
        p = Point{px, py};
    }

    float baseX = path.front().getX();
    float baseY = path.front().getY();

    using clock = std::chrono::steady_clock;
    bool running = true;

    std::vector<std::unique_ptr<Enemy>> el = {};
    Point spawningDirection = ((*path.begin())^(*(++path.begin())));

    // Start the first wave automatically for testing, or rely on UI to trigger it
    // startNextWave(); 
    auto lastTime = clock::now();

    while(running && !wants_to_die_) {
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
            if (enemiesToSpawn_ > 0 && spawnTimer_ >= 1.0f) { // spawn every 1 second
                float offsetSpawn = (rand() / (float)RAND_MAX - 0.5f) * cellSize * 0.3f;
                Point spawnOffset = spawningDirection*offsetSpawn;
                Point spawnPosition{baseX,baseY};
                spawnPosition += spawnOffset;
                
                // Make every 3rd enemy a flying enemy!
                bool is_flying = (enemiesToSpawn_ % 3 == 0);

                const EnemyBlueprint* blueprint = nullptr;
                for(const auto& bp : enemy_catalog_) {
                    if (bp->isFlying() == is_flying) {
                        blueprint = bp.get();
                        break;
                    }
                }

                if (!blueprint) continue; // Should not happen if blueprints are loaded
                el.push_back(blueprint->instantiateEnemy(spawnPosition, offsetSpawn, path.begin(), path.end()));
                addEntity(el.back().get());
                enemiesToSpawn_--;
                spawnTimer_ = 0.0f;
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
             // Update projectiles
            for (auto it = active_projectiles_.begin(); it != active_projectiles_.end(); ) {
                auto& proj = *it;
                proj->live(dt);
                
                if (proj->hasHit()) {
                    std::vector<Enemy*> hit_enemies;
                    if (proj->getSize() > 0.0f) { // Splash damage
                        for (auto& enemy : el) {
                            if (!enemy->isAlive()) continue;
                            Point dir = proj->getPosition() ^ enemy->getPosition();
                            float dist = std::sqrt(dir.getX()*dir.getX() + dir.getY()*dir.getY());
                            if (dist <= proj->getSize()) {
                                hit_enemies.push_back(enemy.get());
                            }
                        }
                    } else { // Single target
                        Enemy* closest = nullptr;
                        float min_dist = 1.0f; // Max acceptable dist for single target splash search
                        for (auto& enemy : el) {
                            if (!enemy->isAlive()) continue;
                            Point dir = proj->getPosition() ^ enemy->getPosition();
                            float dist = std::sqrt(dir.getX()*dir.getX() + dir.getY()*dir.getY());
                            if (dist <= min_dist) {
                                min_dist = dist;
                                closest = enemy.get();
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
            // Update towers with enemy list
            std::vector<Enemy*> raw_el;
            for(auto& e : el) {
                raw_el.push_back(e.get());
            }
            for (auto& tower : placed_towers_) {
                tower->live(dt, raw_el);
                auto new_projs = tower->fetchSpawnedProjectiles();
                for(auto& p : new_projs) {
                    addEntity(p.get());
                    active_projectiles_.push_back(std::move(p));
                }
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
                
                // Clean up remaining projectiles from the wave so they don't hold dangling pointers
                for (auto& proj : active_projectiles_) {
                    removeEntity(proj.get());
                }
                active_projectiles_.clear();
                
                waveActive_ = false;
                std::cout << "Wave " << round_ << " clear! Waiting for next wave...\n";
            }
        }
        
        if (hp_player_ <= 0) {
            std::cout << "GAME OVER!\n";
            running = false;
        }

        // Small sleep to prevent 100% CPU usage loop
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }
}
