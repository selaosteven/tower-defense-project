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
    showUI_ = false;
    selected_tower_ = nullptr;
    selected_cell_.reset();
}

void UI::Session::openBuildUI(Point cell) {
    if (showUI_) closeTowerUI(); // Close any existing UI first

    showUI_ = true;
    selected_cell_ = cell;
    
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

        button->setOnLeftClick([this, blueprint = blueprint.get(), cost]() {
            if (money_ >= cost) {
                money_ -= cost;
                
                float logicX = selected_cell_->getX() + 0.5f;
                float logicY = selected_cell_->getY() + 0.5f;

                Projectile dummyProj(1, 0.5); 
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
        addUISprite(button);
        startY += stepY;
    }
}

void UI::Session::openUpgradeUI(Tower* tower) {
    if (showUI_) closeTowerUI();

    showUI_ = true;
    selected_tower_ = tower;

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

        button->setOnLeftClick([this, upgrade]() {
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

void UI::Session::drawUI(SDL_Renderer* r) {
    if (!showUI_) return;    
    static std::unique_ptr<Sprites::PrimitiveForm> background(Sprites::rectangle({250.0f, 300.0f, 0.0f}, 300.0f, 400.0f, {20, 20, 40, 200}));
    background->draw(r, delta_time_, Point{0.0f, 0.0f}, ui_scale_, 0.0f);
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

    int last_money = money_;
    int last_hp = hp_player_;

    for(int y = 0; y < map_.getHeight(); y++) {
        for(int x = 0; x < map_.getWidth(); x++) {
            
            Case bloc = map_.map_.at(y).at(x);
            Sprites::Sprite* s = nullptr; // On prépare un pointeur vide

            float px = x * cellSize + cellSize / 2.0f;
            float py = y * cellSize + cellSize / 2.0f;

            // 3. ALLOCATION "NEW" : L'objet est créé sur le TAS (Heap).
            // Il ne sera PAS détruit à la sortie du switch ou de la boucle.
            switch (bloc) {
                case Case::Tower:
                    s = Sprites::circle({px, py, cellSize/2}, cellSize/2);
                    break;
                case Case::Path:
                    s = Sprites::rectangle({px, py, cellSize/2}, cellSize/2);
                    break;
                case Case::Wall:
                    s = Sprites::rectangle({px, py, cellSize/2}, cellSize/2);
                    break;
                case Case::Start:
                    s = Sprites::triangle({px, py, cellSize/2}, cellSize/4);
                    break;
                case Case::End:
                    s = Sprites::triangle({px, py, cellSize/2}, cellSize/4);
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

    Enemy ref{5.0f, 1.5f, 0.2f, true}; // Increased LP to 5, and Speed to 1.5 cells per second
    std::vector<Enemy*> el = {};
    Point spawningDirection = ((*path.begin())^(*(++path.begin())));

    // Start the first wave automatically for testing, or rely on UI to trigger it
    startNextWave(); 
    auto lastTime = clock::now();

    while(running && !wants_to_die_) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        if (money_ != last_money) {
            moneyText->setText("Money: " + std::to_string(money_) + "$");
            last_money = money_;
        }
        if (hp_player_ != last_hp) {
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
                
                el.push_back(new Enemy{spawnPosition, offsetSpawn, ref, path.begin(), path.end()});
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
                } else if (enemy->getLp() <= 0) {
                    moneySetter(money_ + 10);
                    enemy->kill();
                }
            }
            
            // Clean up dead enemies
            for (auto it = el.begin(); it != el.end(); ) {
                if (!(*it)->isAlive()) {
                    removeEntity(*it);
                    delete *it;
                    it = el.erase(it);
                } else {
                    ++it;
                }
            }

            // Check if wave is over (no more to spawn and board is clear)
            if (enemiesToSpawn_ <= 0 && el.empty()) {
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
