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
   scene->camera = NULL;
   scene->lights = list_create(20);
   scene->startup_objects = list_create(20);
   scene->back_color = COLOR_BLACK;

   scene->required_model_scopes = list_create(10);
   scene->required_tex_scopes = list_create(10);

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

   list_free(scene->required_model_scopes);
   list_free(scene->required_tex_scopes);

   free(scene);
}