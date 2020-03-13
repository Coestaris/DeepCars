//
// Created by maxim on 11/17/19.
//

#ifdef __GNUC__
#pragma implementation "object.h"
#endif
#include "object.h"

#define O_LOG(format, ...) DC_LOG("object.c", format, __VA_ARGS__)
#define O_ERROR(format, ...) DC_ERROR("object.c", format, __VA_ARGS__)

//
// o_free
//
void o_free(object_t* object)
{
   assert(object);

   if (object->destroy_func)
      object->destroy_func(object);

   DEEPCARS_FREE(object->draw_info);
   DEEPCARS_FREE(object);
}

//
// o_clone
//
object_t* o_clone(object_t* object)
{
   assert(object);

   object_t* new_object = DEEPCARS_MALLOC(sizeof(object_t));
   memcpy(new_object, object, sizeof(object_t));

   new_object->draw_info = DEEPCARS_MALLOC(sizeof(draw_info_t));
   memcpy(new_object->draw_info, object->draw_info, sizeof(draw_info_t));

   return new_object;
}

//
// o_create
//
object_t* o_create()
{
   object_t* object = DEEPCARS_MALLOC(sizeof(object_t));
   object->position = vec3f(0, 0, 0);
   object->rotation = vec3f(0, 0, 0);
   object->scale = vec3f(1, 1, 1);
   object->destroy_func = NULL;
   object->update_func = NULL;
   object->init_func = NULL;
   object->key_event_func = NULL;
   object->mouse_event_func = NULL;
   object->mousemove_event_func = NULL;

   //o_dm_colored(object, COLOR_WHITE);

   object->draw_info = DEEPCARS_MALLOC(sizeof(draw_info_t));
   object->draw_info->model = NULL;
   object->draw_info->draw_normals = false;
   object->draw_info->shadows = true;
   object->draw_info->normal_vao = 0;
   object->draw_info->normal_buffer_len = 0;
   object->draw_info->drawable = false;
   return object;
}

//
// o_enable_draw_normals()
//
void o_enable_draw_normals(object_t* object, vec4 color1, vec4 color2, float len)
{
   assert(object);
   assert(color1);
   assert(color2);

   if(!object->draw_info->normal_vao)
   {
      assert(object->draw_info->model);

      object->draw_info->draw_normals = true;
      m_calculate_normals_vao(object->draw_info->model,
            color1, color2, len,
            &object->draw_info->normal_vao,
            &object->draw_info->normal_buffer_len);
   }
}
