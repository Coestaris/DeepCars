//
// Created by maxim on 11/17/19.
//

#include "drawableObject.h"

drawableObject_t* o_clone(drawableObject_t* object)
{
    drawableObject_t* newObject = malloc(sizeof(drawableObject_t));
    memcpy(newObject, object, sizeof(drawableObject_t));
    return newObject;
}

drawableObject_t* o_create()
{
    drawableObject_t* object = malloc(sizeof(drawableObject_t));
    object->model = NULL;
    object->position = vec3f(0, 0, 0);
    object->modelMat = NULL;
    object->updateFunc = NULL;
    object->initFunc = NULL;
    object->keyEventFunc = NULL;
    object->mouseEventFunc = NULL;
    object->mouseMoveEventFunc = NULL;
    return object;
}