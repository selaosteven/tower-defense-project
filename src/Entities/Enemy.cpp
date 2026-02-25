#include <iostream>
#include "Entities/Enemy.h"

Enemy::Enemy(float lp, float speed, float resistance, bool fly) :
Entity{{0,0}}, lp_{lp}, speed_{speed}, resistance_{resistance}, fly_{fly} {}
