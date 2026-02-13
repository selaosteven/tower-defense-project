#include "PrimitiveForm.h"

namespace Sprite {


PrimitiveForm::PrimitiveForm() : Sprite{} {}

PrimitiveForm::PrimitiveForm(std::array<int, 3> &pos) : Sprite{pos} {}

PrimitiveForm::PrimitiveForm(std::array<int, 3> &pos, PFType type) : Sprite{pos}, type_{type} {}

PrimitiveForm::~PrimitiveForm(){}

void PrimitiveForm::draw(SDL_Surface &win) const {
    
}

}