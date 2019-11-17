//
// Created by maxim on 9/11/19.
//

#ifndef DEEPCARS_CAMERA_H
#define DEEPCARS_CAMERA_H

#include <stdbool.h>
#include "gmath.h"

typedef struct _camera {
    vec4 position;
    vec4 target;
    vec4 direction;
    vec4 up;

    vec4 _cameraRight;
    vec4 _cameraUp;
    mat4 _component1;
    mat4 _component2;

    bool useTarget;

} camera_t;

camera_t* c_create(vec4 position, vec4 up);
void c_free(camera_t* camera);
void c_toMat(mat4 m, camera_t* camera);
void c_rotate(camera_t* camera, float pitch, float yaw);

#endif //DEEPCARS_CAMERA_H
