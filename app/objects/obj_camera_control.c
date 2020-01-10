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
bool  last_cntr         = false;
bool  last_1         = false;

void camera_update_func(object_t* this)
{
   scene_t* scene = scm_get_current();
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

   // shadow_light move
   // left
   if (u_get_key_state(113) == KEY_PRESSED)
      vec4_subv(scene->shadow_light->position, camera_cross_cpy);

   // right
   if (u_get_key_state(114) == KEY_PRESSED)
      vec4_addv(scene->shadow_light->position, camera_cross_cpy);

   // forward
   if (u_get_key_state(111) == KEY_PRESSED)
      vec4_subv(scene->shadow_light->position, camera_dir_cpy);

   // back
   if (u_get_key_state(116) == KEY_PRESSED)
      //vec4_addv(scene->shadow_light->position, camera_dir_cpy);
   {
      if(!last_1)
      {
         switch_ssao();
         last_1 = 1;
      }
   } else last_1 = 0;

   //up
   if (u_get_key_state(65) == KEY_PRESSED)
      camera->position[1] += 0.2f;

   //down
   if (u_get_key_state(50) == KEY_PRESSED)
      camera->position[1] -= 0.2f;

   if (u_get_key_state(105) == KEY_PRESSED)
   {
      if(!last_cntr)
      {
         switch_stages();
         last_cntr = true;
      }
   }
   else last_cntr = false;
}

void camera_key_event_func(object_t* this, uint32_t key, uint32_t state)
{
   if (key == 9) //esc
      u_close();
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
   this->key_event_func = camera_key_event_func;
   this->mouse_event_func = camera_mouse_event_func;
   this->mousemove_event_func = camera_mouse_move_event_func;
   this->destroy_func = camera_destroy_func;

   camera_dir_cpy = cvec4(0, 0, 0, 0);
   camera_cross_cpy = cvec4(0, 0, 0, 0);
   return this;
}