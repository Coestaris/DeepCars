//
// Created by maxim on 1/26/20.
//

#ifdef __GNUC__
#pragma implementation "obj_menu_camera_mover.h"
#endif
#include "obj_menu_camera_mover.h"
#include "../../win_defaults.h"
#include "../../rendering/renderer.h"

static float new_camera_y      = 0;
static float new_camera_radius = 0;

static float camera_y        = 0;
static float camera_radius   = 0;
static bool  updating_camera = false;

static float x_to_y = 0;
static float x_to_r = 0;
static float old_y  = 0;
static float old_r  = 0;
static float x      = 0;

void update_camera(float new_y, float new_r)
{
   new_camera_y = new_y;
   new_camera_radius = new_r;

   x = 0;
   updating_camera = true;

   old_y = camera_y;
   old_r = camera_radius;

   x_to_y = (new_camera_y - camera_y) / 100;
   x_to_r = (new_camera_radius - camera_radius) / 100;
}

float smootherstep(float edge0, float edge1, float x)
{
   return x * x * x * (x * (x * 6 - 15) + 10);
}

static void update_menu_camera_mover(object_t* this)
{
   if(updating_camera)
   {
      float f = smootherstep(0, 1, x += 0.007f);
      camera_y = f * 100 * x_to_y + old_y;
      camera_radius = f * 100 * x_to_r + old_r;

      if(x >= 1)
         updating_camera = false;
   }

   uint64_t time = u_get_frames();
   scene_t* scene = scm_get_current();
   if(time == 1)
   {
      const float lt_off_x = -17.359648f;
      const float lt_off_y = camera_y;
      const float lt_off_z = 6;

      scene->shadow_light->light_camera->target[0] = 0;
      //scene->shadow_light->light_camera->target[1] = 0;
      scene->shadow_light->light_camera->target[2] = 0;

      scene->shadow_light->position[0] = lt_off_x;
      scene->shadow_light->position[1] = lt_off_y;
      scene->shadow_light->position[2] = lt_off_z;
      update_shadow_light();
   }


   camera->position[0] = camera_radius * sinf(time / 1000.0f);
   camera->position[1] = camera_y;
   camera->position[2] = camera_radius * cosf(time / 1000.0f);

   vec4_cpy(camera->direction, camera->position);
  // vec4_mulf(camera->direction, -1);
   vec4_norm(camera->direction);
}

object_t* create_menu_camera_mover()
{
   camera_y = default_camera_y;
   camera_radius = default_camera_r;

   object_t* this = o_create();
   this->update_func = update_menu_camera_mover;
   return this;
}
