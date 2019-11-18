//
// Created by maxim on 11/18/19.
//

#include "obj_cube.h"

drawableObject_t* create_cube(vec3f_t pos, float size, vec4 color)
{
    drawableObject_t* this = o_create();
    this->position = pos;
    this->scale = vec3f(size, size, size);

    this->model = getModel(MODELID_CUBE);
    this->color = color;
    return this;
}