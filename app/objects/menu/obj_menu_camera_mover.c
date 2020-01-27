//
// Created by maxim on 1/26/20.
//

#ifdef __GNUC__
#pragma implementation "obj_menu_camera_mover.h"
#endif
#include "obj_menu_camera_mover.h"
#include "../../win_defaults.h"
#include "../../rendering/renderer.h"

void update_menu_camera_mover(object_t* this)
{
   uint64_t time = u_get_frames();
   scene_t* scene = scm_get_current();
   if(time == 1)
   {
      const float lt_off_x = -17.359648f;
      const float lt_off_y = 50.372597f;
      const float lt_off_z = 6;

      scene->shadow_light->light_camera->target[0] = 0;
      //scene->shadow_light->light_camera->target[1] = 0;
      scene->shadow_light->light_camera->target[2] = 0;

      scene->shadow_light->position[0] = lt_off_x;
      scene->shadow_light->position[1] = lt_off_y;
      scene->shadow_light->position[2] = lt_off_z;
      update_shadow_light();
   }

   camera->position[0] = 102 * sinf(time / 1000.0f);
   camera->position[2] = 102 * cosf(time / 1000.0f);

   vec4_cpy(camera->direction, camera->position);
  // vec4_mulf(camera->direction, -1);
   vec4_norm(camera->direction);
}

object_t* create_menu_camera_mover()
{
   object_t* this = o_create();
   this->update_func = update_menu_camera_mover;
   return this;
}
