#include "Session.h"

#include "Sprites/PrimitiveForm.h"


Session::Session(std::string name_map): 
    UI::Window{},
    map_{name_map},
    map_ope_{std::vector<Entity>{}, map_.getWidth(), map_.getHeight()},
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

            // 3. ALLOCATION "NEW" : L'objet est créé sur le TAS (Heap).
            // Il ne sera PAS détruit à la sortie du switch ou de la boucle.
            switch (bloc) {
                case Case::Tower:
                    s = new Sprites::PrimitiveForm(Sprites::rectangle({x*34.0f, y*34.0f, 10.0f}, 10.0f));
                    break;
                case Case::Path:
                    s = new Sprites::PrimitiveForm(Sprites::circle({x*34.0f, y*34.0f, 10.0f}, 10.0f));
                    break;
                case Case::Wall:
                    s = new Sprites::PrimitiveForm(Sprites::triangle({x*34.0f, y*34.0f, 10.0f}, 10.0f));
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
}