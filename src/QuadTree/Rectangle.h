#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"

class Rectangle {
private:
    int x_;
    int y_;
    int w_;
    int h_;

public:
    Rectangle(int x, int y , int w ,int h);
    bool contains(Point point);
    bool checkOverlap(Point center, float range);

    inline int getX() const {return x_;}
    inline int getY() const {return y_;}
    inline int getW() const {return w_;}
    inline int getH() const {return h_;}
};



#endif