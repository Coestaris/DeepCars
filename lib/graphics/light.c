//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "shadow_light.h"
#endif
#include "light.h"

#define L_LOG(format, ...) DC_LOG("shadow_light.c", format, __VA_ARGS__)
#define L_ERROR(format, ...) DC_ERROR("shadow_light.c", format, __VA_ARGS__)

light_t* l_create()
{
   light_t* lt = malloc(sizeof(light_t));
   lt->color = cvec4(1,1,1,1);
   lt->position = cvec4(0,0,0,1);

   lt->light_proj = NULL;
   lt->light_view = NULL;
   lt->light_space = NULL;
   lt->light_camera = NULL;

   return lt;
}

void l_free(light_t* light)
{
   if(light->light_view) mat4_free(light->light_view);
   if(light->light_proj) mat4_free(light->light_proj);
   if(light->light_space) mat4_free(light->light_space);
   if(light->light_camera) c_free(light->light_camera);

   vec4_free(light->color);
   vec4_free(light->position);
   free(light);
}