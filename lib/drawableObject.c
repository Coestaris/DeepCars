//
// Created by maxim on 11/17/19.
//

#include "drawableObject.h"

void o_free(drawableObject_t* object)
{
    if(object->destroyFunc)
        object->destroyFunc(object);

    free(object);
}

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
    object->rotation = vec3f(0, 0, 0);
    object->scale = vec3f(1, 1, 1);
    object->destroyFunc = NULL;
    object->updateFunc = NULL;
    object->initFunc = NULL;
    object->keyEventFunc = NULL;
    object->mouseEventFunc = NULL;
    object->mouseMoveEventFunc = NULL;
    return object;
}