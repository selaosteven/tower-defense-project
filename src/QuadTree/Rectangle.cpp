#include "Rectangle.h"
#include <algorithm>

Rectangle::Rectangle(float x , float y , float w , float h) 
    : x_{x}, y_{y}, w_{w}, h_{h} {}

bool Rectangle::contains(Point point) {
    float x = point.getX();
    float y = point.getY();

    return (x >= x_ - w_/2 && x <= x_ + w_/2 &&
            y >= y_ - h_/2 && y <= y_ + h_/2);
}

bool Rectangle::checkOverlap(Point center, float range){

    float halfW = w_ / 2.0f;
    float halfH = h_ / 2.0f;

    // Find the nearest point of the rectangle to the center of circle
    float closestX = std::max(x_ - halfW, std::min((float)center.getX(), x_ + halfW));
    float closestY = std::max(y_ - halfH, std::min((float)center.getY(), y_ + halfH));

    // Distance between the point and the center of circle
    float dx = center.getX() - closestX;
    float dy = center.getY() - closestY;

    // If distance <= radius => intersection
    return dx * dx + dy * dy <= range * range;

}
