#ifndef PRIMITIVEFORM_H
#define PRIMITIVEFORM_H

#include "Sprite.h"

namespace Sprite
{

enum primitiveFormTypes
{
    Square=0,
    Triangle,
    Circle,
};
using PFType = enum primitiveFormTypes ;


class PrimitiveForm : public Sprite
{
    private:
        PFType type_;
        int 

    public:
    PrimitiveForm();
    PrimitiveForm(std::array<int, 3> &pos);
    PrimitiveForm(std::array<int, 3> &pos, PFType type);
    ~PrimitiveForm();

    void draw(SDL_Surface &win) const override;
};

}

#endif