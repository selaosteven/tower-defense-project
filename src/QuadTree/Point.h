#ifndef POINT_H
#define POINT_H

class Point {
private:
    int x_; // Cordonnée x du point 
    int y_; // Cordonnée y du point

public:
    Point(int x,int y);
    inline int getX() const {return x_;}
    inline int getY() const {return y_;}

};



#endif