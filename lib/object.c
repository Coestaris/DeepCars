//
// Created by maxim on 11/17/19.
//

#ifdef __GNUC__
#pragma implementation "object.h"
#endif
#include "object.h"

// for color
#include "graphics/graphics.h"

//
// o_free
//
void o_free(object_t* object)
{
   if (object->destroy_func)
      object->destroy_func(object);

   free(object->draw_info);
   free(object);
}

//
// o_clone
//
object_t* o_clone(object_t* object)
{
   object_t* new_object = malloc(sizeof(object_t));
   memcpy(new_object, object, sizeof(object_t));
   switch(new_object->draw_mode)
   {
      case DM_COLORED:
         new_object->draw_info = malloc(sizeof(draw_info_colored_t));
         memcpy(new_object->draw_info, object->draw_info, sizeof(draw_info_colored_t));
         break;
      case DM_COLORED_SHADED:
         new_object->draw_info = malloc(sizeof(draw_info_colored_shaded_t));
         memcpy(new_object->draw_info, object->draw_info, sizeof(draw_info_colored_shaded_t));
         break;
         break;
      case DM_TEXTURED:
         new_object->draw_info = malloc(sizeof(draw_info_textured_t));
         memcpy(new_object->draw_info, object->draw_info, sizeof(draw_info_textured_t));
         break;
      case DM_TEXTURED_SHADED:
         new_object->draw_info = malloc(sizeof(draw_info_textured_shaded_t));
         memcpy(new_object->draw_info, object->draw_info, sizeof(draw_info_textured_shaded_t));
         break;
   }

   return new_object;
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

   object->draw_info = NULL;
   o_dm_colored(object, COLOR_WHITE);
   return object;
}

//
// o_dm_colored()
//
void o_dm_colored(object_t* object, vec4 color)
{
   if(object->draw_info) free(object->draw_info);

   object->draw_mode = DM_COLORED;
   object->draw_info = malloc(sizeof(draw_info_colored_t));
   draw_info_colored_t* info = (draw_info_colored_t*)object->draw_info;
   info->color = color;
}

//
// o_dm_colored()
//
void o_dm_colored_shaded(object_t* object, vec4 color, float ambient)
{
   if(object->draw_info) free(object->draw_info);

   object->draw_mode = DM_COLORED_SHADED;
   object->draw_info = malloc(sizeof(draw_info_colored_shaded_t));
   draw_info_colored_shaded_t* info = (draw_info_colored_shaded_t*)object->draw_info;
   info->color = color;
   info->ambient = ambient;
}

//
// o_dm_textured()
//
void o_dm_textured(object_t* object, texture_t* texture)
{
   if(object->draw_info) free(object->draw_info);

   object->draw_mode = DM_TEXTURED;
   object->draw_info = malloc(sizeof(draw_info_textured_t));
   draw_info_textured_t* info = (draw_info_textured_t*)object->draw_info;
   info->texture = texture;
}

//
// o_dm_textured_shaded()
//
void o_dm_textured_shaded(object_t* object, texture_t* diffuse, texture_t* specular, texture_t* emit)
{
   if(object->draw_info) free(object->draw_info);

   object->draw_mode = DM_TEXTURED_SHADED;
   object->draw_info = malloc(sizeof(draw_info_textured_shaded_t));
   draw_info_textured_shaded_t* info = (draw_info_textured_shaded_t*)object->draw_info;
   info->diffuse = diffuse;
   info->specular = specular;
   info->emit = emit;
}