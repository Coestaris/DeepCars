//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_LIGHT_H
#define DEEPCARS_LIGHT_H

#include "gmath.h"
#include "camera.h"

typedef struct _light
{
   float radius;

   float constant;
   float linear;
   float quadratic;

   vec4 color;
   vec4 position;

} light_t;

typedef struct _shadow_light {

   vec4 position;
   vec4 direction;

   float brightness;

   mat4 light_view;
   mat4 light_proj;
   mat4 light_space;
   camera_t* light_camera;

} shadow_light_t;

shadow_light_t* l_sh_create(vec4 position, vec4 up);
void l_sh_free(shadow_light_t* light);

void l_calc_radius(light_t* light);
light_t* l_create(void);
void l_free(light_t* light);

#endif //DEEPCARS_LIGHT_H
