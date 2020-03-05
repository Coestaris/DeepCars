//
// Created by maxim on 9/11/19.
//

#ifndef DEEPCARS_CAMERA_H
#define DEEPCARS_CAMERA_H

#include "../coredefs.h"

#include "gmath.h"

// Represents data for an easy way to convert camera parameters
// such as position, camera target to a view matrix
typedef struct _camera
{
   // Position of a camera
   vec4 position;

   // Camera target (optional)
   vec4 target;

   // Camera direction (used if no target is specified)
   vec4 direction;

   // Camera upward direction (used for calculations)
   vec4 up;

   // Use target instead of direction for calculations
   bool use_target;

   // Some private buffer components
   vec4 _camera_right;
   vec4 _camera_up;
   mat4 _component_1;
   mat4 _component_2;

} camera_t;

// camera_t constructor
camera_t* c_create(vec4 position, vec4 up);

// Frees all used by camera resources
void c_free(camera_t* camera);

// Calculates view matrix from camera objects
void c_to_mat(mat4 m, camera_t* camera);

// Rotates camera using pitch and yaw angles. 'use_target' should be false
void c_rotate(camera_t* camera, float pitch, float yaw);

#endif //DEEPCARS_CAMERA_H
