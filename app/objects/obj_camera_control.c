//
// Created by maxim on 11/18/19.
//

#include "obj_camera_control.h"

float cameraPitch = 0;
float cameraYaw = M_PI / 2;

float dx;
float dy;

float lastdx = 0;
float lastdy = 0;

void camera_updateFunc(struct _drawableObject* this)
{

}

void camera_keyEventFunc(struct _drawableObject* this, uint32_t key, uint32_t state)
{
    if(key == 9) //esc
        u_close();
}

void camera_mouseEventFunc(struct _drawableObject* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{

}

void camera_mouseMoveEventFunc(struct _drawableObject* this, uint32_t x, uint32_t y)
{

}

drawableObject_t* create_cameraControl()
{
    drawableObject_t* this = o_create();
    this->updateFunc = camera_updateFunc;
    this->keyEventFunc = camera_keyEventFunc;
    this->mouseEventFunc = camera_mouseEventFunc;
    this->mouseMoveEventFunc = camera_mouseMoveEventFunc;
    return this;
}