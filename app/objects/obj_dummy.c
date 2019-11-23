//
// Created by maxim on 11/18/19.
//

#include "obj_dummy.h"

object_t* create_dummy(vec3f_t pos, float size, vec4 color, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->model = model;
   o_dm_simple(this, color);
   return this;
}

object_t* create_textured_dummy(vec3f_t pos, float size, texture_t* texture, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->model = model;
   o_dm_textured(this, texture);
   return this;
}