//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_DRAWABLEOBJECT_H
#define DEEPCARS_DRAWABLEOBJECT_H

#include "resources/model.h"

struct _object;

//
// Object callback types
//
typedef void (* update_func_t)(struct _object* this);
typedef void (* key_event_func_t)(struct _object* this, uint32_t key, uint32_t state);
typedef void (* mouse_event_func_t)(struct _object* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);
typedef void (* mousemove_event_func_t)(struct _object* this, uint32_t x, uint32_t y);

// Struct that describes drawable object
typedef struct _object
{
   // Model to be rendered
   model_t* model;

   // Transformation of object
   vec3f_t position;
   vec3f_t rotation;
   vec3f_t scale;

   // Base object callbacks
   update_func_t update_func;
   update_func_t init_func;
   update_func_t destroy_func;

   // Object event callbacks. If set to 0 updater will ignore it
   key_event_func_t key_event_func;
   mouse_event_func_t mouse_event_func;
   mousemove_event_func_t mousemove_event_func;

   //todo: delete
   vec4 color;

} object_t;

// Clone object and allocates another one
object_t* o_clone(object_t* object);

// object_t constructor
object_t* o_create();

// Frees object and all its used resources
void o_free(object_t* object);

#endif //DEEPCARS_DRAWABLEOBJECT_H
