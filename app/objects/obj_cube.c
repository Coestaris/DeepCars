//
// Created by maxim on 11/18/19.
//

#include "obj_cube.h"

drawableObject_t* create_cube(vec3f_t pos, double size, material_t* mat)
{
    drawableObject_t* this = o_create();
    this->position = pos;

    return this;
}