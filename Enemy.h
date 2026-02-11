#ifndef ENEMY_H
#define ENEMY_H

class Enemy{
protected:
    // Stats de chaque ennemies
    double lp; // Life Point
    double speed; // Speed
    double resistance; // Resistance
    bool fly; // Fly
   
public:
    Enemy(double lp, double speed, double resistance, bool fly); // constructeur
};

#endif