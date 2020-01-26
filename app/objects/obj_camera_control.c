//
// Created by maxim on 11/18/19.
//

#include "obj_camera_control.h"
#include "../rendering/renderer.h"

float dx;
float dy;
float press_mouse_x;
float press_mouse_y;
vec4  camera_dir_cpy;
vec4  camera_cross_cpy;
float camera_pitch      = 0;
float camera_yaw        = M_PI / 2;
float lastdx            = 0;
float lastdy            = 0;
vec4 player_start;

void camera_update_func(object_t* this)
{
   uint64_t frame = u_get_frames();

   if(frame == 1)
      vec4_cpy(player_start, camera->position);

   camera_pitch = dy;
   camera_yaw = dx + (float)(M_PI / 2.0);
   c_rotate(camera, camera_pitch, camera_yaw);

   vec4_cpy(camera_dir_cpy, camera->direction);
   vec4_mulf(camera_dir_cpy, .2f);

   vec4_cpy(camera_cross_cpy, camera->_camera_right);
   vec4_mulf(camera_cross_cpy, .2f);

   //a
   if (u_get_key_state(38) == KEY_PRESSED)
      vec4_subv(camera->position, camera_cross_cpy);

   //d
   if (u_get_key_state(40) == KEY_PRESSED)
      vec4_addv(camera->position, camera_cross_cpy);

   //w
   if (u_get_key_state(25) == KEY_PRESSED)
      vec4_subv(camera->position, camera_dir_cpy);

   //s
   if (u_get_key_state(39) == KEY_PRESSED)
      vec4_addv(camera->position, camera_dir_cpy);

   //up
   if (u_get_key_state(65) == KEY_PRESSED)
      camera->position[1] += 0.2f;

   //down
   if (u_get_key_state(50) == KEY_PRESSED)
      camera->position[1] -= 0.2f;

   if(frame % 15 == 0)
   {
      scene_t* scene = scm_get_current();
      const float diffX = camera->position[0] - player_start[0];
      const float diffY = camera->position[1] - player_start[1];
      const float diffZ = camera->position[2] - player_start[2];

      const float lt_off_x = -17.359648f;
      const float lt_off_y = 50.372597f;
      const float lt_off_z = 6;

      scene->shadow_light->light_camera->target[0] = diffX;
      //scene->shadow_light->light_camera->target[1] = 0;
      scene->shadow_light->light_camera->target[2] = diffZ;

      scene->shadow_light->position[0] = diffX + lt_off_x;
      scene->shadow_light->position[1] = lt_off_y;
      scene->shadow_light->position[2] = diffZ + lt_off_z;
      update_shadow_light();
   }
}

void camera_mouse_event_func(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if (state == MOUSE_RELEASE)
   {
      lastdx = dx;
      lastdy = dy;
   }
   else
   {
      press_mouse_x = x;
      press_mouse_y = y;
   }
}

void camera_mouse_move_event_func(object_t* this, uint32_t x, uint32_t y)
{
   if (u_get_mouse_state(MOUSE_LEFT) == MOUSE_PRESSED)
   {
      dx = ((float) x - (float) press_mouse_x) / 100.0f + lastdx;
      dy = ((float) y - (float) press_mouse_y) / 100.0f + lastdy;
   }
}

void camera_destroy_func(object_t* this)
{
   if (camera_dir_cpy)
   {
      vec4_free(camera_dir_cpy);
      camera_dir_cpy = NULL;
      vec4_free(camera_cross_cpy);
   }
}

object_t* create_camera_control()
{
   object_t* this = o_create();
   this->update_func = camera_update_func;
   this->mouse_event_func = camera_mouse_event_func;
   this->mousemove_event_func = camera_mouse_move_event_func;
   this->destroy_func = camera_destroy_func;

   camera_dir_cpy = cvec4(0, 0, 0, 0);
   camera_cross_cpy = cvec4(0, 0, 0, 0);
   player_start = cvec4(0, 0, 0, 0);
   return this;
}