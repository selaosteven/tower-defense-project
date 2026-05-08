#include "Session.h"
#include <thread>

#include <iostream>
#include <thread>
#include "Sprites/PrimitiveForm.h"
#include "Sprites/Text.h"
#include "Sprites/Button.h"
#include "Entities/Enemy.h"
#include "Entities/Tower.h"
#include "Entities/TowerTree.h"
#include "Entities/Projectile.h"


UI::Session::Session(std::string name_map): 
    UI::Window{},
    map_{name_map},
    map_ope_{map_.getWidth(), map_.getHeight()},
    hp_player_{50},
    round_{0},
    waveActive_{false},
    enemiesToSpawn_{0},
    spawnTimer_{0.0f},
    money_{1000},
    showUI_{false},
    selected_cell_{},
    ticks_per_seconds_{120},
    selected_tower_{nullptr}
    {   
        tower_catalog_.push_back(TowerTree::loadFromFile("../src/Ressources/sniper.json"));
        tower_catalog_.push_back(TowerTree::loadFromFile("../src/Ressources/basic.json"));
        tower_catalog_.push_back(TowerTree::loadFromFile("../src/Ressources/antiair.json"));
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
    for (auto* sprite : active_ui_elements_) {
        removeUISprite(sprite);
        delete sprite; // The UI owns these temporary sprites
    }
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
    
    float startY = 110.0f;
    float stepY = 60.0f;

    // Title
    auto title = new Sprites::Text({110.0f, startY, 11.0f}, "Build Tower", Sprites::Text::POKETEXT, 24, {255, 255, 255, 255});
    active_ui_elements_.push_back(title);
    addUISprite(title);
    startY += 40;

    for (const auto& blueprint : tower_catalog_) {
        int cost = blueprint->getRootUpgrade() ? blueprint->getRootUpgrade()->cost : 0;
        std::string label = blueprint->getTowerType() + " (" + std::to_string(cost) + "$)";

        auto button = new Sprites::Button({120.0f, startY, 10.0f}, 260.0f, 50.0f);
        button->addSubSprite(Sprites::rectangle({130.0f, 25.0f, 0.0f}, 260.0f, 50.0f, {80, 80, 150, 255}));
        
        auto text = new Sprites::Text({15.0f, 15.0f, 1.0f}, label, Sprites::Text::POKETEXT, 18, {255, 255, 255, 255});
        button->addSubSprite(text);

        button->setOnLeftClick([this, blueprint = blueprint.get(), cost, cell = *selected_cell_]() {
            if (money_ >= cost) {
                money_ -= cost;
                
                float logicX = cell.getX() + 0.5f;
                float logicY = cell.getY() + 0.5f;

                Projectile dummyProj(0.0f, 5.0f); 
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
    
}

void UI::Session::openUpgradeUI(Tower* tower) {
    if (showUI_) closeTowerUI();

    // Show tower range
    showUI_ = true;
    selected_tower_ = tower;
    selected_tower_->setShowRange(true);
    menu_button_index_ = 0;
    menu_buttons_.clear();

    float startY = 110.0f;
    float stepY = 60.0f;

    auto title = new Sprites::Text({110.0f, startY, 11.0f}, "Upgrades", Sprites::Text::POKETEXT, 24, {255, 255, 255, 255});
    active_ui_elements_.push_back(title);
    addUISprite(title);
    startY += 40;

    const UpgradeNode* current_node = tower->getCurrentUpgradeNode();
    if (!current_node || current_node->children.empty()) {
        auto text = new Sprites::Text({120.0f, startY, 1.0f}, "No upgrades available.", Sprites::Text::POKETEXT, 18, {255, 255, 255, 255});
        active_ui_elements_.push_back(text);
        addUISprite(text);
        return;
    }

    for (const auto& upgrade_node_ptr : current_node->children) {
        const UpgradeNode* upgrade = upgrade_node_ptr.get();
        std::string label = upgrade->name + " (" + std::to_string(static_cast<int>(upgrade->cost)) + "$)";

        auto button = new Sprites::Button({120.0f, startY, 10.0f}, 260.0f, 50.0f);
        button->addSubSprite(Sprites::rectangle({130.0f, 25.0f, 0.0f}, 260.0f, 50.0f, {80, 80, 150, 255}));
        
        auto text = new Sprites::Text({15.0f, 15.0f, 1.0f}, label, Sprites::Text::POKETEXT, 18, {255, 255, 255, 255});
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
        startY += stepY;
    }
    
}

void UI::Session::clickLeft(Point click) {
    // If the UI is open, clicks on UI buttons are handled by the buttons themselves.
    // We only need to check for clicks *outside* the UI panel to close it.
    if (showUI_) {
        SDL_Rect uiRect = { 100, 100, 300, 400 };

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


void UI::Session::drawUI(SDL_Renderer* r) {
    if (!showUI_) return;    
    static std::unique_ptr<Sprites::PrimitiveForm> background(Sprites::rectangle({250.0f, 300.0f, 0.0f}, 300.0f, 400.0f, {20, 20, 40, 200}));
    background->draw(r, delta_time_, Point{0.0f, 0.0f}, ui_scale_, 0.0f);
    
    // Draw selection highlight on the currently selected menu button
    if (!menu_buttons_.empty() && menu_button_index_ < menu_buttons_.size()) {
        auto selected_button = menu_buttons_[menu_button_index_];
        
        // Draw a bright border around the selected button
        auto pos = selected_button->getPosition();
        SDL_Color col = {255, 200, 0, 255}; // Gold color
        // The width and height are mapped from the original dimensions, with added spacing so the highlight is visibly framing the button boundaries 
        Session::drawHighlightBox(r, delta_time_, Point{0.0f, 0.0f}, ui_scale_, pos.getX() - 4.0f, pos.getY() - 4.0f, 260.0f + 8.0f, 50.0f + 8.0f, 2.0f, col);
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
    std::unique_ptr<Sprites::PrimitiveForm> top(Sprites::rectangle({x + w / 2.0f, y + thickness / 2.0f, 999.0f}, w, thickness, col));
    std::unique_ptr<Sprites::PrimitiveForm> bottom(Sprites::rectangle({x + w / 2.0f, y + h - thickness / 2.0f, 999.0f}, w, thickness, col));
    std::unique_ptr<Sprites::PrimitiveForm> left(Sprites::rectangle({x + thickness / 2.0f, y + h / 2.0f, 999.0f}, thickness, h, col));
    std::unique_ptr<Sprites::PrimitiveForm> right(Sprites::rectangle({x + w - thickness / 2.0f, y + h / 2.0f, 999.0f}, thickness, h, col));

    top->draw(r, dt, offset, scale, 0.0f);
    bottom->draw(r, dt, offset, scale, 0.0f);
    left->draw(r, dt, offset, scale, 0.0f);
    right->draw(r, dt, offset, scale, 0.0f);
}


void UI::Session::mainSession() {
    while(!Window::sdl_initiated);
    float cellWidth  = getWinWidth() / static_cast<float>(map_.getWidth());
    float cellHeight = getWinHeight() / static_cast<float>(map_.getHeight());
    scale_ = std::min(cellWidth, cellHeight);
    float cellSize = 1.0f;

    // Center the map on the screen for the rendering engine
    float offsetX = (getWinWidth()  - scale_ * map_.getWidth())  / 2.0f;
    float offsetY = (getWinHeight() - scale_ * map_.getHeight()) / 2.0f;
    camera_position_ = Point{offsetX, offsetY};


    // Status UI Elements
    auto moneyText = new Sprites::Text({20.0f, 20.0f, 10.0f}, "Money: " + std::to_string(money_) + "$", Sprites::Text::POKETEXT, 24, {255, 215, 0, 255});
    addUISprite(moneyText);
    
    auto hpText = new Sprites::Text({getWinWidth() / 2.0f, -40.0f, 10.0f}, "HP: " + std::to_string(hp_player_), Sprites::Text::POKETEXT, 24, {255, 50, 50, 255}, true);
    addUISprite(hpText);

    // START WAVE BUTTON
    auto bouton_next_wave = new Sprites::Button({60.0f, -60, 10.0f}, 260.0f, 50.0f);
    auto text = new Sprites::Text({15.0f, 15.0f, 1.0f}, "START WAVE", Sprites::Text::POKETEXT, 18, {255, 255, 255, 255});
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
            Sprites::Sprite* s = nullptr; // On prépare un pointeur vide

            float px = x * cellSize + cellSize / 2.0f;
            float py = y * cellSize + cellSize / 2.0f;

            // 3. ALLOCATION "NEW" : L'objet est créé sur le TAS (Heap).
            // Il ne sera PAS détruit à la sortie du switch ou de la boucle.
            switch (bloc) {
                case Case::Tower:
                    s = Sprites::circle({px, py, 99}, cellSize/2);
                    break;
                case Case::Path:
                    s = Sprites::rectangle({px, py, 99}, cellSize/2);
                    break;
                case Case::Wall:
                    s = Sprites::rectangle({px, py, 99}, cellSize/2);
                    break;
                case Case::Start:
                    s = Sprites::triangle({px, py, 99}, cellSize/4);
                    break;
                case Case::End:
                    s = Sprites::triangle({px, py, 99}, cellSize/4);
                    break;
                
                default:
                    continue; // On passe au suivant si c'est du vide
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
    auto lastTickTime = clock::now();
    auto lastSpawnTime = clock::now();
    bool running = true;

    Enemy ground_ref{5.0f, 1.5f, 0.2f, false}; // Standard Ground enemy
    Enemy flying_ref{3.0f, 2.0f, 0.1f, true};  // Fast flying enemy with slightly lower LP
    std::vector<Enemy*> el = {};
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
                const Enemy& spawn_ref = is_flying ? flying_ref : ground_ref;
                
                el.push_back(new Enemy{spawnPosition, offsetSpawn, spawn_ref, path.begin(), path.end()});
                addEntity(el.back());
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
                    removeEntity(enemy);
                } else if (enemy->getLp() <= 0) {
                    moneySetter(money_ + 10);
                    enemy->kill();
                    removeEntity(enemy);
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
                                hit_enemies.push_back(enemy);
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
            // Update towers with enemy list
            for (auto& tower : placed_towers_) {
                tower->live(dt, el);
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
                for (auto it = el.begin(); it != el.end(); ) {
                    delete *it;
                    it = el.erase(it);
                }
                
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
