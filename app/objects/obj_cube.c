//
// Created by maxim on 11/18/19.
//

#include "obj_cube.h"

object_t* create_cube(vec3f_t pos, float size, vec4 color)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->model = get_model(MODELID_CUBE);
   this->color = color;
   return this;
}