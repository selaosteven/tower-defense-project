#include <SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Session.h"

// Choisira si on lance la session ou le mode editeur
int main(int argc, char *argv[]){
    if(argc > 1){
        Session test{argv[1]};
        test.mainSession();
    }
    return 0;
}