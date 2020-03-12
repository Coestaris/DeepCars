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

// Structs that describes drawing options of object
typedef struct _draw_info
{
   // Describes whether the object will be drawn
   bool drawable;

   // Model to be rendered
   model_t* model;

   // Describes whether the object will be cast shadows
   bool shadows;

   // Material of the model
   material_t* material;

   // Describes whether the normals will be drawn
   bool draw_normals;

   // Normal buffer properties
   GLuint normal_vao;
   size_t normal_buffer_len;

} draw_info_t;

// Struct that describes objects
typedef struct _object
{
   // Transformations of object
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

// Calculates VAO buffers of model normals with specified lengths
void o_enable_draw_normals(object_t* object, vec4 color1, vec4 color2, float len);

#endif //DEEPCARS_OBJECT_H
