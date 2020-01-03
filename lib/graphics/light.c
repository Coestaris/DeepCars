//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "light.h"
#endif
#include "light.h"

#define L_LOG(format, ...) DC_LOG("light.c", format, __VA_ARGS__)
#define L_ERROR(format, ...) DC_ERROR("light.c", format, __VA_ARGS__)

light_t* l_create()
{
   light_t* lt = malloc(sizeof(light_t));
   lt->color = cvec4(1,1,1,1);

   lt->light_proj = cmat4();
   lt->light_view = cmat4();
   lt->light_space = cmat4();

   return lt;
}

void l_free(light_t* light)
{
   mat4_free(light->light_view);
   mat4_free(light->light_proj);
   mat4_free(light->light_space);
   c_free(light->light_camera);

   vec4_free(light->color);
   vec4_free(light->position);
   free(light);
}