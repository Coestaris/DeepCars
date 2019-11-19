//
// Created by maxim on 11/17/19.
//

#ifdef __GNUC__
#pragma implementation "scene.h"
#endif
#include "scene.h"

//
// sc_create
//
scene_t* sc_create(uint32_t id)
{
   scene_t* scene = malloc(sizeof(scene_t));
   scene->id = id;
   scene->camera = c_create(
           cvec4(0, 0, 0, 0),
           cvec4(0, 1, 0, 0));

   scene->lights = list_create(20);
   scene->startup_objects = list_create(20);

   return scene;
}

//
// sc_free
//
void sc_free(scene_t* scene)
{
   c_free(scene->camera);
   for (size_t i = 0; i < scene->lights->count; i++)
      l_free((light_t*) scene->lights->collection[i]);

   for (size_t i = 0; i < scene->startup_objects->count; i++)
      o_free((object_t*) scene->startup_objects->collection[i]);

   list_free(scene->lights);
   list_free(scene->startup_objects);

   free(scene);
}