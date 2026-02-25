#ifndef TARGET_H
#define TARGET_H
#include "Entities/Entity.h"
#include "QuadTree/Point.h"

class Target
{
public:
    virtual ~Target() {}
    virtual Point getPosition() const = 0;
};

class TargetPoint : public Target
{
private:
    Point pos_;
public:
    TargetPoint(Point pos) : pos_{pos} {}
    inline Point getPosition() const override{
        return pos_;
    }
};

class TargetEntity : public Target {
private:
    const Entity* entity_;
public:
    TargetEntity(const Entity * target) : entity_{target} {} 
    inline Point getPosition() const override {
        return entity_ ? entity_->getPosition() : Point{0,0};
    }
};

#endif
