//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "sceneManager.h"
#endif
#include "sceneManager.h"

// List of all pushed scenes
list_t* scenes;
int current_scene = -1;

//
// scm_get_current
//
scene_t* scm_get_current(void)
{
   return (scene_t*) scenes->collection[current_scene];
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
   if (current_scene != -1)
   {
      //todo
   }

   current_scene = id;
   u_clear_objects(free);
   scene_t* scene = (scene_t*) scenes->collection[id];

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