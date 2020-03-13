//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "shadow_light.h"
#endif
#include "light.h"

#define L_LOG(format, ...) DC_LOG("shadow_light.c", format, __VA_ARGS__)
#define L_ERROR(format, ...) DC_ERROR("shadow_light.c", format, __VA_ARGS__)

//
// l_create()
//
light_t* l_create()
{
   light_t* lt = DEEPCARS_MALLOC(sizeof(light_t));
   lt->color = cvec4(1,1,1,1);
   lt->position = cvec4(0,0,0,1);

   return lt;
}

//
// l_free()
//
void l_free(light_t* light)
{
   ASSERT(light);

   vec4_free(light->color);
   vec4_free(light->position);
   DEEPCARS_FREE(light);
}

//
// l_sh_create()
//
shadow_light_t* l_sh_create(vec4 position, vec4 up)
{
   ASSERT(position);
   ASSERT(up);

   shadow_light_t* lt = DEEPCARS_MALLOC(sizeof(shadow_light_t));
   lt->light_proj = cmat4();
   lt->light_view = cmat4();
   lt->light_space = cmat4();
   lt->light_camera = c_create(vec4_ccpy(position), up);

   lt->position = position;
   return lt;
}

//
// l_sh_free()
//
void l_sh_free(shadow_light_t* light)
{
   ASSERT(light);

   if(light->light_view) mat4_free(light->light_view);
   if(light->light_proj) mat4_free(light->light_proj);
   if(light->light_space) mat4_free(light->light_space);
   if(light->light_camera) c_free(light->light_camera);

   vec4_free(light->position);
   DEEPCARS_FREE(light);
}

//
// l_calc_radius()
//
void l_calc_radius(light_t* light)
{
   ASSERT(light);

   float max_brightness = fmaxf(fmaxf(light->color[0], light->color[1]), light->color[2]);
   light->radius = (-light->linear
                    + sqrtf(light->linear * light->linear -
                           4 * light->quadratic *
                           (light->constant - (256.0f / 5.0f) * max_brightness)))
                  / (2.0f * light->quadratic);
}