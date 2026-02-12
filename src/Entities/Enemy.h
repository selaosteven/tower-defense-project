#ifndef ENEMY_H
#define ENEMY_H

class Enemy{
protected:
    // Stats de chaque ennemies
    double lp_; // Life Point
    double speed_; // Speed
    double resistance_; // Resistance
    bool fly_; // Fly
   
public:
    Enemy(double lp, double speed, double resistance, bool fly); // constructeur
};

#endif