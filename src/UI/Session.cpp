#include "Session.h"

#include "Sprites/PrimitiveForm.h"
#include "Sprites/Text.h"
#include "Sprites/Button.h"
#include "Entities/Enemy.h"
#include "Entities/TowerTree.h"

UI::Session::Session(std::string name_map): 
    UI::Window{},
    map_{name_map},
    map_ope_{map_.getWidth(), map_.getHeight()},
    hp_player_{50},
    round_{0},
    money_{0},
    showUI_{false}
    {}

void UI::Session::moneySetter(int new_money) {
    hp_player_ = new_money;
}

void UI::Session::hpSetter(int new_hp) {
    hp_player_ = new_hp;
}

void UI::Session::clickLeft(Point click) {

    // Si l'UI est ouverte
    if (showUI_) {

        SDL_Rect uiRect = { 100, 100, 300, 200 };

        // Si clic DANS l'UI → on ne ferme pas
        if (click.getX() >= uiRect.x &&
            click.getX() <= uiRect.x + uiRect.w &&
            click.getY() >= uiRect.y &&
            click.getY() <= uiRect.y + uiRect.h)
        {
            // Ici tu gères les boutons si tu veux
            std::cout << "Clic dans l'UI\n";
            return;
        }

        // Sinon → clic hors UI → on ferme
        showUI_ = false;
        std::cout << "UI fermée\n";
        return;
    }
    
    float seuil = 0.5f; // seuil logique

    // 1) Recalcul EXACT du offset (scale_ est déjà correct)
    float offsetX = (getWinWidth()  - scale_ * map_.getWidth())  / 2.0f;
    float offsetY = (getWinHeight() - scale_ * map_.getHeight()) / 2.0f;

    // 2) Conversion du clic pixel → logique
    float clickLX = (click.getX() - offsetX) / scale_;
    float clickLY = (click.getY() - offsetY) / scale_;

    float verticalFix = 1.6f;   
    clickLY += verticalFix;

    // 3) Parcours des sprites
    for (Sprites::Sprite* s : getSprites()) {

        Point p = s->getPosition(); // position logique (ex : 3.5, 4.5)
        float sx = p.getX();
        float sy = p.getY();

        // 4) Distance logique
        float dx = sx - clickLX;
        float dy = sy - clickLY;

        if (dx*dx + dy*dy > seuil * seuil)
            continue;

        // 5) Conversion logique → case
        int cellX = (int)std::floor(sx);
        int cellY = (int)std::floor(sy);

        // 6) Lecture
        Case type = map_.map_[cellY][cellX];
        map_.printCase(type);

        if (type == Case::Tower) {
            showUI_ = true;
        }
    }
}

void UI::Session::drawUI(SDL_Renderer* r) {
    if (!showUI_) return;

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 180);

    SDL_Rect panel = { 100, 100, 300, 200 };
    SDL_RenderFillRect(r, &panel);
}


void UI::Session::mainSession() {
    while(!Window::sdl_initiated);
    float cellWidth  = getWinWidth() / static_cast<float>(map_.getWidth());
    float cellHeight = getWinHeight() / static_cast<float>(map_.getHeight());
    scale_ = std::min(cellWidth, cellHeight);
    float cellSize = 1.0f;
    // offset_ = Point{offsetX, offsetY}; // Décommentez si vous avez ajouté offset_ dans Session.h
    auto myProj = Projectile(1, 0.5);;
    auto sniperBlueprint = TowerTree::loadFromFile("../src/Ressources/sniper.json");
    auto myTower = sniperBlueprint->instantiateTower({100, 100}, myProj);
    Sprites::Text* myText = new Sprites::Text(
        {20.0f, 20.0f, 10.0f}, // Position in pixels (Top Left)
        "Top Left UI Text",
        "../src/Ressources/PokemonClassic.ttf", // Path to a valid font
        20, // Font Size
        {255, 125, 255, 255}, // Color
        200, // Max width of the bounding box
        false // Centered
    );
    addUISprite(myText);

    Sprites::Text* myText2 = new Sprites::Text(
        {-420.0f, 20.0f, 10.0f}, // Negative X anchors to the right side
        "Right-Anchored UI Text",
        "../src/Ressources/POKPIX1.TTF", // Path to a valid font
        40, // Font Size
        {125, 125, 255, 255}, // Color
        400, // Max width of the bounding box
        false // Centered
    );
    addUISprite(myText2);

    // Button acts as the master entity. Give it the absolute/anchored coordinates.
    auto boutonTest = new Sprites::Button({-420.0f, 80.0f, 10.0f}, 150.0f, 50.0f);
    
    // The sub-sprites are relative to the Button. 
    // 1. Add a background to see the button bounds (Primitive rectangle origins are centered, so we offset by width/2, height/2)
    boutonTest->addSubSprite(Sprites::rectangle({75.0f, 25.0f, 0.0f}, 150.0f, 50.0f, {80, 80, 150, 255}));
    
    // 2. Add the text centered over the button background
    boutonTest->addSubSprite(new Sprites::Text(
        {75.0f, 25.0f, 1.0f}, // Relative to button center
        "CLICK ME",
        "../src/Ressources/PokemonClassic.ttf", // Path to a valid font
        20, // Font Size
        {255, 255, 255, 255}, // Color
        150, // Max width of the bounding box
        true // Centered
    ));
    
    boutonTest->setOnLeftClick([]() { std::cout << "Button clicked!" << std::endl; });
    addUISprite(boutonTest);

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
    auto lastTime = clock::now();
    bool running = true;
    Sprites::Sprite* s = nullptr; // On prépare un pointeur vide

    Enemy ref{0.2, .02, 0.2, true};
    std::vector<Enemy*> el = {};
    Point spawningDirection = ((*path.begin())^(*(++path.begin())));
    while(running) {
        float offsetSpawn = (rand() / (float)RAND_MAX - 0.5f) * cellSize * 0.3f;
        Point spawnOffset = spawningDirection*offsetSpawn;
        Point spawnPosition{baseX,baseY};
        spawnPosition += spawnOffset;
        auto now = clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
        for (auto enemy : el) {
            enemy->live(delta_time_);            
        }
        if (dt >= 2000) { // toutes les 100 ms
            el.push_back(new Enemy{spawnPosition, offsetSpawn, ref, path.begin()});
            addEntity(el.back());
            std::cout << el.size() << " -- " << std::endl;
    
            lastTime = now;
        }
    }
}
