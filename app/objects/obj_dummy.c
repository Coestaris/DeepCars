//
// Created by maxim on 11/18/19.
//

#include "obj_dummy.h"

void update(object_t* this)
{
   this->rotation.y += 0.01;
}

object_t* create_colored_shaded_dummy(vec3f_t pos, float size, float ambient, vec4 color, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);
   this->update_func = update;

   this->draw_info->drawable = true;
   this->draw_info->model = model;
   this->draw_info->object_color = COLOR_RED;
   //this->model = model;
   //o_dm_colored_shaded(this, color, ambient);
   return this;
}

object_t* create_colored_dummy(vec3f_t pos, float size, vec4 color, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->draw_info->drawable = true;
   this->draw_info->model = model;
   this->draw_info->object_color = COLOR_GREEN;
   //this->model = model;
   //o_dm_colored(this, color);
   return this;
}

object_t* create_textured_dummy(vec3f_t pos, float size, texture_t* texture, model_t* model)
{
   object_t* this = o_create();
   this->position = pos;
   this->scale = vec3f(size, size, size);

   this->draw_info->drawable = true;
   this->draw_info->model = model;
   this->draw_info->object_color = COLOR_NAVY;
   //o_dm_textured(this, texture);
   return this;
}