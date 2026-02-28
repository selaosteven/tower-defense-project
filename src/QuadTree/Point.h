#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <algorithm>

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
    const Point operator-(const Point& p){
        return Point{x_-p.x_, y_-p.y_};
    }

    const Point operator^(const Point& p){
        return Point{p.x_-x_, p.y_-y_};
    }

    const Point operator|(const double max){
        float nx = x_ < -max ? -max : (x_ > max ? max : x_);
        float ny = y_ < -max ? -max : (y_ > max ? max : y_);
        return {nx, ny};
    }
    
    const Point& operator+=(const Point& p){
        x_ += p.x_;        
        y_ += p.y_;
        return *this; 
    }

    Point operator*(float f) const {
        return Point(x_ * f, y_ * f);
    }

    friend std::ostream& operator<<(std::ostream &out, const Point &c)
    {
        out << "(" << c.x_ << ',' << c.y_ << ")";
        return out;
    }
};



#endif