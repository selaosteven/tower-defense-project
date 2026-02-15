#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"

class Rectangle {
private:
    float x_;
    float y_;
    float w_;
    float h_;

public:
    Rectangle(float x, float y , float w , float h);
    bool contains(Point point);
    bool checkOverlap(Point center, float range);

    inline float getX() const {return x_;}
    inline float getY() const {return y_;}
    inline float getW() const {return w_;}
    inline float getH() const {return h_;}
};



#endif