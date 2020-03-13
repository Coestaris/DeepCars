//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_LIGHT_H
#define DEEPCARS_LIGHT_H

#include "../coredefs.h"

#include "gmath.h"
#include "camera.h"

// Describes scene point light. Scene can contain an unlimited count of lights
typedef struct _light
{
   // Precalculated radius of area that could potentially be lit by this light
   float radius;

   // Quadratic parameters of light area
   float constant;
   float linear;
   float quadratic;

   // Light color
   vec4 color;
   // Light position
   vec4 position;

} light_t;

// Describes scene spot light that cast shadow. Scene can contain only one shadow_light
typedef struct _shadow_light
{
   // Position of the light
   vec4 position;

   // Light brightness
   float brightness;

   // Light space transforms
   mat4 light_view;
   mat4 light_proj;
   mat4 light_space;

   camera_t* light_camera;

} shadow_light_t;

// shadow_light_t constructor
shadow_light_t* l_sh_create(vec4 position, vec4 up);

// Frees shadow_light_t
void l_sh_free(shadow_light_t* light);

// Calculates light's area radius
void l_calc_radius(light_t* light);

// light_t constructor
light_t* l_create(void);

// Frees light
void l_free(light_t* light);

#endif //DEEPCARS_LIGHT_H
