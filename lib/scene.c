//
// Created by maxim on 11/17/19.
//

#ifdef __GNUC__
#pragma implementation "scene.h"
#endif
#include "scene.h"

#define SC_LOG(format, ...) DC_LOG("scene.c", format, __VA_ARGS__)
#define SC_ERROR(format, ...) DC_ERROR("scene.c", format, __VA_ARGS__)

//
// sc_create
//
scene_t* sc_create(uint32_t id)
{
   scene_t* scene = DEEPCARS_MALLOC(sizeof(scene_t));
   scene->id = id;
   scene->shadow_light = NULL;
   scene->backcolor = NULL;
   scene->lights = list_create();
   scene->startup_objects = list_create();

   return scene;
}

//
// sc_free
//
void sc_free(scene_t* scene)
{
   if(scene->shadow_light) l_sh_free(scene->shadow_light);
   for (size_t i = 0; i < scene->lights->count; i++)
      l_free((light_t*) scene->lights->collection[i]);

   for (size_t i = 0; i < scene->startup_objects->count; i++)
      o_free((object_t*) scene->startup_objects->collection[i]);

   if(scene->backcolor)
      vec4_free(scene->backcolor);

   list_free(scene->lights);
   list_free(scene->startup_objects);
   DEEPCARS_FREE(scene);
}