#ifndef POINT_H
#define POINT_H

#include <iostream>

class Point {
private:
    float x_; // Cordonnée x du point 
    float y_; // Cordonnée y du point

public:
    Point(float x,float y);
    inline float getX() const {return x_;}
    inline float getY() const {return y_;}
    inline bool operator==(Point p){
        return x_ == p.x_ && y_ == p.y_;
    }
    void print() const;

    const Point operator+(const Point& p){
        return Point{x_+p.x_,y_+p.y_};
    }
    
    const Point& operator+=(const Point& p){
        x_ += p.x_;        
        y_ += p.y_;
        return *this; 
    }
};



#endif