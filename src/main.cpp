#include <SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "UI/Session.h"

// Choisira si on lance la session ou le mode editeur
int main(int argc, char *argv[]){
    std::string map = (argc > 1) ? argv[1] : "../src/Ressources/map.txt";
    UI::Session test{map};
    test.mainSession();
    test.waitForClose();
    
    return 0;
}