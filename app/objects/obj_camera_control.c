//
// Created by maxim on 11/18/19.
//

#include "obj_camera_control.h"

float cameraPitch = 0;
float cameraYaw = M_PI / 2;
float dx;
float dy;
float pressMouseX;
float pressMouseY;
float lastdx = 0;
float lastdy = 0;
vec4 cameraDirCpy;
vec4 cameraCrossCpy;

void camera_updateFunc(object_t* this)
{
   scene_t* scene = scm_get_current();
   cameraPitch = dy;
   cameraYaw = dx + (float) M_PI / 2.0;
   identityMat(view);

   c_rotate(scene->camera, cameraPitch, cameraYaw);
   c_toMat(view, scene->camera);

   vec4_cpy(cameraDirCpy, scene->camera->direction);
   vec4_mulf(cameraDirCpy, .2f);

   vec4_cpy(cameraCrossCpy, scene->camera->_cameraRight);
   vec4_mulf(cameraCrossCpy, .2f);

   //left
   if (u_get_key_state(38) == KEY_PRESSED)
      vec4_subv(scene->camera->position, cameraCrossCpy);

   //right
   if (u_get_key_state(40) == KEY_PRESSED)
      vec4_addv(scene->camera->position, cameraCrossCpy);

   //forward
   if (u_get_key_state(25) == KEY_PRESSED)
      vec4_subv(scene->camera->position, cameraDirCpy);

   //back
   if (u_get_key_state(39) == KEY_PRESSED)
      vec4_addv(scene->camera->position, cameraDirCpy);

   //up
   if (u_get_key_state(65) == KEY_PRESSED)
      scene->camera->position[1] += 0.2f;
   //down
   if (u_get_key_state(50) == KEY_PRESSED)
      scene->camera->position[1] -= 0.2f;
}

void camera_keyEventFunc(object_t* this, uint32_t key, uint32_t state)
{
   if (key == 9) //esc
      u_close();
}

void camera_mouseEventFunc(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if (state == MOUSE_RELEASE)
   {
      lastdx = dx;
      lastdy = dy;
   }
   else
   {
      pressMouseX = x;
      pressMouseY = y;
   }
}

void camera_mouseMoveEventFunc(object_t* this, uint32_t x, uint32_t y)
{
   if (u_get_mouse_state(MOUSE_LEFT) == MOUSE_PRESSED)
   {
      dx = ((float) x - (float) pressMouseX) / 100.0f + lastdx;
      dy = ((float) y - (float) pressMouseY) / 100.0f + lastdy;
   }
}

void camera_destroyFunc(object_t* this)
{
   if (cameraDirCpy)
   {
      freeVec4(cameraDirCpy);
      cameraDirCpy = NULL;
      freeVec4(cameraCrossCpy);
   }
}

object_t* create_cameraControl()
{
   object_t* this = o_create();
   this->update_func = camera_updateFunc;
   this->key_event_func = camera_keyEventFunc;
   this->mouse_event_func = camera_mouseEventFunc;
   this->mousemove_event_func = camera_mouseMoveEventFunc;
   this->destroy_func = camera_destroyFunc;

   cameraDirCpy = cvec4(0, 0, 0, 0);
   cameraCrossCpy = cvec4(0, 0, 0, 0);
   return this;
}