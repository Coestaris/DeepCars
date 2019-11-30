//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "light.h"
#endif
#include "light.h"

#define L_LOG(format, ...) DC_LOG("light.c", format, __VA_ARGS__)
#define L_ERROR(format, ...) DC_ERROR("light.c", format, __VA_ARGS__)

light_t* l_create(light_type_t type)
{
   light_t* lt = malloc(sizeof(light_t));
   lt->type = type;
   lt->color = cvec4(0, 0, 0, 0);
   lt->direction = cvec4(0, 0, 0, 0);
   lt->point = cvec4(0, 0, 0, 0);
   return lt;
}

void l_free(light_t* light)
{
   vec4_free(light->point);
   vec4_free(light->color);
   vec4_free(light->direction);
   free(light);
}