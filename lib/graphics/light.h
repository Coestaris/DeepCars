//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_LIGHT_H
#define DEEPCARS_LIGHT_H

#include "gmath.h"

typedef enum _light_type
{
   LT_DIRECTION,
   LT_POINT,
   LT_SPOTLIGHT,

} light_type_t;

typedef struct _light
{
   light_type_t type;

   vec4 color;
   float brightness;

   vec4 direction;
   vec4 point;

} light_t;

light_t* l_create(light_type_t type);
void l_free(light_t* light);

#endif //DEEPCARS_LIGHT_H
