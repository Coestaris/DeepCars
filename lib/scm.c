//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "scm.h"
#endif
#include "scm.h"

#include "resources/txm.h"

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

// Returns true if scope required by specified scene
bool scm_is_tex_scope_required(scene_t* scene, int scope)
{
   for(size_t i = 0; i < scene->required_tex_scopes->count; i++)
      if(((size_t)scene->required_tex_scopes->collection[i]) == scope)
         return true;
   return false;
}

//
// scm_load_scene
//
void scm_load_scene(uint32_t id, bool free)
{
   scene_t* scene =  NULL;

   // find scene with specified id in list
   for(size_t i = 0; i < scenes->count; i++)
      if(((scene_t*)scenes->collection[i])->id == id)
         scene = (scene_t*) scenes->collection[id];

   if(!scene)
   {
      printf("[sceneManager.c][ERROR]: Unable to find scene with id %i\n", id);
      exit(EXIT_FAILURE);
   }

   current_scene = scene;
   u_clear_objects(free);

   //todo: model scope

   // unload all no longer required scopes and load required ones
   for(size_t i = 0; i < MAX_LOADED_SCOPES; i++)
   {
      if(loaded_scopes[i] && !scm_is_tex_scope_required(current_scene, i))
         txm_unload_scope(i);
      else if(!loaded_scopes[i] && scm_is_tex_scope_required(current_scene, i))
         txm_load_scope(i);
   }

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