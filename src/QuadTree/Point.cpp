#include "Point.h"

Point::Point(float x , float y) :
    x_{x}, y_{y} {}

void Point::print() const{
    std::cout << "Point de cordonnée : x = " << x_ << " et y = " << y_ << "\n" << std::endl;
}