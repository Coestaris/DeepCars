//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_OBJECT_H
#define DEEPCARS_OBJECT_H

#include "resources/model.h"

struct _object;

//
// Object callback types
//
typedef void (* update_func_t)(struct _object* this);
typedef void (* key_event_func_t)(struct _object* this, uint32_t key, uint32_t state);
typedef void (* mouse_event_func_t)(struct _object* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);
typedef void (* mousemove_event_func_t)(struct _object* this, uint32_t x, uint32_t y);

typedef enum _draw_mode {
   DM_COLORED,
   DM_COLORED_SHADED,
   DM_TEXTURED,
   DM_TEXTURED_SHADED

} draw_mode_t;

typedef struct _draw_info_simple {
   vec4 color;

} draw_info_colored_t;

typedef struct _draw_info_simple_shaded {
   vec4 color;
   float ambient;

} draw_info_colored_shaded_t;


typedef struct _draw_info_textured {
   texture_t* texture;

} draw_info_textured_t;

typedef struct _draw_info_textured_shaded {
   texture_t* diffuse;
   texture_t* specular;
   texture_t* emit;

} draw_info_textured_shaded_t;

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

   // Shader data for drawing object
   void* draw_info;

   // Shader to used for drawing object
   draw_mode_t draw_mode;

} object_t;

// Clone object and allocates another one
object_t* o_clone(object_t* object);

// object_t constructor
object_t* o_create();

// Frees object and all its used resources
void o_free(object_t* object);

// Sets rendering parameters of object
void o_dm_colored(object_t* object, vec4 color);
void o_dm_colored_shaded(object_t* object, vec4 color, float ambient);
void o_dm_textured(object_t* object, texture_t* texture);
void o_dm_textured_shaded(object_t* object, texture_t* diffuse, texture_t* specular, texture_t* emit);

#endif //DEEPCARS_OBJECT_H
