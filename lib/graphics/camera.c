//
// Created by maxim on 9/11/19.
//

#ifdef __GNUC__
#pragma implementation "camera.h"
#endif
#include "camera.h"

#define C_LOG(format, ...) DC_LOG("camera.c", format, __VA_ARGS__)
#define C_ERROR(format, ...) DC_ERROR("camera.c", format, __VA_ARGS__)

//
// c_create()
//
camera_t* c_create(vec4 position, vec4 up)
{
   camera_t* camera = DEEPCARS_MALLOC(sizeof(camera_t));
   camera->position = position;
   camera->up = up;
   camera->direction = cvec4(0, 0, 0, 0);
   camera->target = NULL;

   camera->use_target = false;

   camera->_camera_right = cvec4(0, 0, 0, 0);
   camera->_camera_up = cvec4(0, 0, 0, 0);
   camera->_component_1 = cmat4();
   camera->_component_2 = cmat4();

   return camera;
}

//
// c_free()
//
void c_free(camera_t* camera)
{
   if (camera->position)
      vec4_free(camera->position);
   if (camera->target)
      vec4_free(camera->target);
   if (camera->direction)
      vec4_free(camera->direction);
   vec4_free(camera->up);

   vec4_free(camera->_camera_right);
   vec4_free(camera->_camera_up);
   mat4_free(camera->_component_1);
   mat4_free(camera->_component_2);

   DEEPCARS_FREE(camera);
}

//
// c_to_mat()
//
void c_to_mat(mat4 m, camera_t* camera)
{
   if (camera->use_target)
   {
      vec4_cpy(camera->direction, camera->position);

      vec4_subv(camera->direction, camera->target);
      vec4_norm(camera->direction);
   }

   vec4_cpy(camera->_camera_right, camera->up);

   vec4_cross(camera->_camera_right, camera->direction);
   vec4_norm(camera->_camera_right);

   vec4_cpy(camera->_camera_up, camera->direction);
   vec4_cross(camera->_camera_up, camera->_camera_right);

   mat4_fill(camera->_component_1,
             camera->_camera_right[0], camera->_camera_right[1], camera->_camera_right[2], 0,
             camera->_camera_up[0], camera->_camera_up[1], camera->_camera_up[2], 0,
             camera->direction[0], camera->direction[1], camera->direction[2], 0,
             0, 0, 0, 1);

   mat4_fill(camera->_component_2,
             1, 0, 0, -camera->position[0],
             0, 1, 0, -camera->position[1],
             0, 0, 1, -camera->position[2],
             0, 0, 0, 1);

   mat4_identity(m);
   mat4_mulm(m, camera->_component_1);
   mat4_mulm(m, camera->_component_2);
}

//
// c_rotate()
//
void c_rotate(camera_t* camera, float pitch, float yaw)
{
   camera->direction[0] = cosf(pitch) * cosf(yaw);
   camera->direction[1] = sinf(pitch);
   camera->direction[2] = cosf(pitch) * sinf(yaw);
}