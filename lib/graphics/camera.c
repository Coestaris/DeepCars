//
// Created by maxim on 9/11/19.
//

#include "camera.h"

camera_t* c_create(vec4 position, vec4 up)
{
    camera_t* camera = malloc(sizeof(camera_t));
    camera->position = position;
    camera->up = up;
    camera->direction = cvec4(0, 0, 0, 0);
    camera->target = NULL;

    camera->useTarget = false;

    camera->_cameraRight = cvec4(0, 0, 0, 0);
    camera->_cameraUp    = cvec4(0, 0, 0, 0);
    camera->_component1  = cmat4();
    camera->_component2  = cmat4();

    return camera;
}

void c_free(camera_t* camera)
{
    if(camera->position)
        freeVec4(camera->position);
    if(camera->target)
        freeVec4(camera->target);
    if(camera->direction)
        freeVec4(camera->direction);
    freeVec4(camera->up);

    freeVec4(camera->_cameraRight);
    freeVec4(camera->_cameraUp);
    freeMat4(camera->_component1);
    freeMat4(camera->_component2);

    free(camera);
}

void c_toMat(mat4 m, camera_t* camera)
{
    if(camera->useTarget)
    {
        vec4_cpy(camera->direction, camera->position);

        vec4_subv(camera->direction, camera->target);
        vec4_norm(camera->direction);
    }

    vec4_cpy(camera->_cameraRight, camera->up);

    vec4_cross(camera->_cameraRight, camera->direction);
    vec4_norm (camera->_cameraRight);

    vec4_cpy  (camera->_cameraUp, camera->direction);
    vec4_cross(camera->_cameraUp, camera->_cameraRight);

    fillMat4(camera->_component1,
             camera->_cameraRight[0], camera->_cameraRight[1], camera->_cameraRight[2], 0,
             camera->_cameraUp   [0], camera->_cameraUp   [1], camera->_cameraUp   [2], 0,
             camera->direction   [0], camera->direction   [1], camera->direction   [2], 0,
             0,              0,              0, 1);

    fillMat4(camera->_component2,
             1, 0, 0, -camera->position[0],
             0, 1, 0, -camera->position[1],
             0, 0, 1, -camera->position[2],
             0, 0, 0, 1);

    identityMat(m);
    mat4_mulm(m, camera->_component1);
    mat4_mulm(m, camera->_component2);
}

void c_rotate(camera_t* camera, float pitch, float yaw)
{
    camera->direction[0] = cosf(pitch) * cosf(yaw);
    camera->direction[1] = sinf(pitch);
    camera->direction[2] = cosf(pitch) * sinf(yaw);
}