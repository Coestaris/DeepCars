//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_DRAWABLEOBJECT_H
#define DEEPCARS_DRAWABLEOBJECT_H

#include "resources/model.h"

struct _drawableObject;

typedef void (*updateFunc_t)(struct _drawableObject* this);
typedef void (*keyEventFunc_t)(struct _drawableObject* this, uint32_t key, uint32_t state);
typedef void (*mouseEventFunc_t)(struct _drawableObject* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);
typedef void (*mouseMoveEventFunc_t)(struct _drawableObject* this, uint32_t x, uint32_t y);

typedef struct _drawableObject {
    model_t* model;
    vec3f_t position;
    mat4 modelMat;

    updateFunc_t updateFunc;
    updateFunc_t initFunc;

    keyEventFunc_t keyEventFunc;
    mouseEventFunc_t mouseEventFunc;
    mouseMoveEventFunc_t mouseMoveEventFunc;

} drawableObject_t;

drawableObject_t* o_clone(drawableObject_t* object);
drawableObject_t* o_create();
void o_free(drawableObject_t* object);

#endif //DEEPCARS_DRAWABLEOBJECT_H
