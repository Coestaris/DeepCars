//
// Created by maxim on 11/17/19.
//

#ifdef __GNUC__
#pragma implementation "object.h"
#endif
#include "object.h"

//
// o_free
//
void o_free(object_t* object)
{
   if (object->destroy_func)
      object->destroy_func(object);

   free(object);
}

//
// o_clone
//
object_t* o_clone(object_t* object)
{
   object_t* newObject = malloc(sizeof(object_t));
   memcpy(newObject, object, sizeof(object_t));
   return newObject;
}

//
// o_create
//
object_t* o_create()
{
   object_t* object = malloc(sizeof(object_t));
   object->model = NULL;
   object->position = vec3f(0, 0, 0);
   object->rotation = vec3f(0, 0, 0);
   object->scale = vec3f(1, 1, 1);
   object->destroy_func = NULL;
   object->update_func = NULL;
   object->init_func = NULL;
   object->key_event_func = NULL;
   object->mouse_event_func = NULL;
   object->mousemove_event_func = NULL;
   return object;
}