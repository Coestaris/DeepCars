//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_LIGHT_H
#define DEEPCARS_LIGHT_H

#include "gmath.h"
#include "camera.h"

typedef struct _light
{
   vec4 color;
   vec4 position;

   mat4 light_view;
   mat4 light_proj;
   mat4 light_space;
   camera_t* light_camera;

} light_t;

light_t* l_create();
void l_free(light_t* light);

#endif //DEEPCARS_LIGHT_H
