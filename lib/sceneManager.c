//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "sceneManager.h"
#endif
#include "sceneManager.h"

// List of all pushed scenes
list_t* scenes;
scene_t* current_scene = NULL;

//
// scm_get_current
//
scene_t* scm_get_current(void)
{
   return current_scene;
}

//
// scm_init
//
void scm_init()
{
   scenes = list_create(10);
}

//
// scm_push_scene
//
void scm_push_scene(scene_t* scene)
{
   list_push(scenes, scene);
}

//
// scm_load_scene
//
void scm_load_scene(uint32_t id, bool free)
{
   if (current_scene)
   {
      //todo
   }

   scene_t* scene =  NULL;

   // find scene with specified id in list
   for(size_t i = 0; i < scenes->count; i++)
      if(((scene_t*)scenes->collection[i])->id == id)
         scene = (scene_t*) scenes->collection[id];

   if(!scene)
   {
      printf("[sceneManager.c][ERROR]: Unable to find scene with id %i", id);
      exit(1);
   }

   current_scene = scene;
   u_clear_objects(free);

   for (size_t i = 0; i < scene->startup_objects->count; i++)
      u_push_object(o_clone((object_t*) scene->startup_objects->collection[i]));
}

//
// scm_free
//
void scm_free()
{
   for (size_t i = 0; i < scenes->count; i++)
      sc_free((scene_t*) scenes->collection[i]);
   list_free(scenes);
}