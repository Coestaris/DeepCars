//
// Created by maxim on 11/18/19.
//

#include "obj_dummy.h"

object_t* create_textured_dummy(vec3 pos, float size, material_t* material, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->draw_info->drawable = true;
   this->draw_info->shadows = true;
   this->draw_info->model = model;
   this->draw_info->material = material;
   //o_enable_draw_normals(this, COLOR_GREEN, COLOR_WHITE, 6 / size);
   return this;
}