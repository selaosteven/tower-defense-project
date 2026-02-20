#include <SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Session.h"

// Choisira si on lance la session ou le mode editeur
int main(int argc, char *argv[]){

    std::string filename = argv[1]; // Pour recuperer le nom de la map en .txt

    Session s1{filename};
    s1.mainSession();
    

    return 0;
}