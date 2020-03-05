//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_OBJECT_H
#define DEEPCARS_OBJECT_H

#include "coredefs.h"
#include "resources/model.h"

struct _object;

//
// Object callback types
//
typedef void (* update_func_t)(struct _object* this);
typedef void (* key_event_func_t)(struct _object* this, uint32_t key, uint32_t state);
typedef void (* mouse_event_func_t)(struct _object* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);
typedef void (* mousemove_event_func_t)(struct _object* this, uint32_t x, uint32_t y);

typedef struct _draw_info
{
   bool drawable;

   // Model to be rendered
   model_t* model;

   // Cast shadows?
   bool shadows;

   material_t* material;

   bool draw_normals;
   GLuint normal_vao;
   size_t normal_buffer_len;

} draw_info_t;

// Struct that describes drawable object
typedef struct _object
{
   // Transformation of object
   vec3 position;
   vec3 rotation;
   vec3 scale;

   // Base object callbacks
   update_func_t update_func;
   update_func_t init_func;
   update_func_t destroy_func;

   // Object event callbacks. If set to 0 updater will ignore it
   key_event_func_t key_event_func;
   mouse_event_func_t mouse_event_func;
   mousemove_event_func_t mousemove_event_func;

   // Shader data for drawing object
   draw_info_t* draw_info;

} object_t;

// Clone object and allocates another one
object_t* o_clone(object_t* object);

// object_t constructor
object_t* o_create();

// Frees object and all its used resources
void o_free(object_t* object);

void o_enable_draw_normals(object_t* object, vec4 color1, vec4 color2, float len);

#endif //DEEPCARS_OBJECT_H
