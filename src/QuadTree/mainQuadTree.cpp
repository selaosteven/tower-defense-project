#include <iostream>
#include "QuadTree.h"
#include "Rectangle.h"
#include "Point.h"

int main() {

    // Rectangle centré en (200,200), largeur 200, hauteur 200
    Rectangle boundary(200, 200, 200, 200);
    QuadTree qt(boundary);

    std::cout << "=== TEST REPARTITION 10 POINTS ===" << std::endl;

    // 10 points répartis dans tous les quadrants
    Point pts[] = {
        {150,150}, {160,140}, // TL
        {250,150}, {240,130}, // TR
        {150,250}, {140,260}, // BL
        // {250,250}, {260,240}, // BR
        // {200,200},            // centre
        // {210,210}             // proche centre
    };

    for (const auto& p : pts)
        qt.insert(p);

    // Affichage ASCII complet
    qt.print();

    return 0;
}