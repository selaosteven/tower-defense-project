#include <SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "UI/Session.h"

// Choisira si on lance la session ou le mode editeur
int main(int argc, char *argv[]){
    if(argc > 1){
        UI::Session test{argv[1]};
        test.mainSession();
        test.waitForClose();
    }
    return 0;
}