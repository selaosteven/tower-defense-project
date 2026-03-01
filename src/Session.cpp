#include "Session.h"

#include "Sprites/PrimitiveForm.h"
#include "Entities/Enemy.h"

Session::Session(std::string name_map): 
    UI::Window{},
    map_{name_map},
    map_ope_{map_.getWidth(), map_.getHeight()},
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

void Session::mainSession() {

    float cellWidth  = getWinWidth() / static_cast<float>(map_.getWidth());
    float cellHeight = getWinHeight() / static_cast<float>(map_.getHeight());
    float cellSize   = std::min(cellWidth, cellHeight);
    float offsetX = (getWinWidth()  - cellSize * map_.getWidth())  / 2.0f;
    float offsetY = (getWinHeight() - cellSize * map_.getHeight()) / 2.0f;


    // 1. LE COFFRE-FORT : Ce tableau survit à la fin de la fonction.
    // Il garde tes objets 'Sprite' en vie dans la RAM.
    static std::vector<Sprites::Sprite*> mapSprites;

    // 2. NETTOYAGE : Si tu relances la fonction, on vide l'ancien test
    // pour éviter de faire exploser la mémoire.
    for(auto s : mapSprites) {
        delete s; 
    }
    mapSprites.clear();

    for(int y = 0; y < map_.getHeight(); y++) {
        for(int x = 0; x < map_.getWidth(); x++) {
            
            Case bloc = map_.map_.at(y).at(x);
            Sprites::Sprite* s = nullptr; // On prépare un pointeur vide

            float px = offsetX + x * cellSize;
            float py = offsetY + y * cellSize;

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
                    s = Sprites::triangle({px, py, cellSize/2}, cellSize/2);
                    break;
                case Case::End:
                    s = Sprites::triangle({px, py, cellSize/2}, cellSize/2);
                    break;
                
                default:
                    continue; // On passe au suivant si c'est du vide
            }

            if (s) {
                // 4. SAUVEGARDE : On stocke l'adresse dans le tableau statique
                mapSprites.push_back(s);
                
                // 5. ENVOI AU MOTEUR : Ton addSprite reçoit un pointeur VALIDE
                addSprite(s); 
            }
        }
    }

     // Spawn des enemies
    std::list<Point> path = map_.getPath();

    for(auto& p : path) {
        float px = offsetX + p.getY() * cellSize + cellSize / 2.0f;
        float py = offsetY + p.getX() * cellSize + cellSize / 2.0f;
        p = Point{px, py};
    }

    float baseX = path.front().getX();
    float baseY = path.front().getY();

    using clock = std::chrono::steady_clock;
    auto lastTime = clock::now();
    bool running = true;
    Sprites::Sprite* s = nullptr; // On prépare un pointeur vide

    Enemy ref{0.2,0.2,0.2,true, cellSize};
    std::vector<Enemy*> el = {};
    Point spawningDirection = ((*path.begin())^(*(++path.begin()))) * (1.0f/cellSize);
    while(running) {
        float offsetSpawn = (rand() / (float)RAND_MAX - 0.5f) * cellSize * 0.3f;
        Point spawnOffset = spawningDirection*offsetSpawn;
        Point spawnPosition{baseX,baseY};
        spawnPosition += spawnOffset;
        auto now = clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
        for(auto e : el) e->live(delta_time_);
        if (dt >= 2000) { // toutes les 100 ms
            el.push_back(new Enemy{spawnPosition, offsetSpawn, ref, path.begin()});
            addEntity(el.back());
            std::cout << el.size() << " -- " << std::endl;
    
            lastTime = now;
        }
    }
}