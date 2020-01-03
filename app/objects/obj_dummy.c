//
// Created by maxim on 11/18/19.
//

#include "obj_dummy.h"

void update(object_t* this)
{
   this->rotation.y += 0.01;
}

object_t* create_textured_dummy(vec3f_t pos, float size, material_t* material, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->draw_info->drawable = true;
   this->draw_info->model = model;
   this->draw_info->material = material;
   return this;
}