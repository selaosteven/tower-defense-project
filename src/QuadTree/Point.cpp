#include "Point.h"

Point::Point(int x , int y) :
    x_{x}, y_{y} {}

void Point::print() const{
    std::cout << "Point de cordonnée : x = " << x_ << " et y = " << y_ << "\n" << std::endl;
}