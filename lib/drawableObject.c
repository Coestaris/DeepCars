//
// Created by maxim on 11/17/19.
//

#include "drawableObject.h"

void o_free(drawableObject_t* object)
{
    free(object->modelMat);
    free(object);
}

drawableObject_t* o_clone(drawableObject_t* object)
{
    drawableObject_t* newObject = malloc(sizeof(drawableObject_t));
    memcpy(newObject, object, sizeof(drawableObject_t));

    mat4 newModelMat = cmat4();
    memcpy(newModelMat, object->modelMat, sizeof(float) * 4 * 4);

    newObject->modelMat = newModelMat;
    return newObject;
}

drawableObject_t* o_create()
{
    drawableObject_t* object = malloc(sizeof(drawableObject_t));
    object->model = NULL;
    object->position = vec3f(0, 0, 0);
    object->modelMat = cmat4();
    identityMat(object->modelMat);

    object->updateFunc = NULL;
    object->initFunc = NULL;
    object->keyEventFunc = NULL;
    object->mouseEventFunc = NULL;
    object->mouseMoveEventFunc = NULL;
    return object;
}