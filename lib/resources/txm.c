//
// Created by maxim on 11/23/19.
//

#ifdef __GNUC__
#pragma implementation "txm.h"
#endif
#include "txm.h"

#include <string.h>
#include "../structs.h"

// Contains data necessary for indexing textures in a list
typedef struct _txm_node {
   size_t id;
   size_t scope;
   texture_t* tex;

} txm_node_t;

// All textures stored in a txm
list_t* textures;
bool loaded_scopes[MAX_LOADED_SCOPES];

//
// txm_init()
//
void txm_init()
{
   memset(loaded_scopes, 0, sizeof(bool) * MAX_LOADED_SCOPES);
   textures = list_create(10);
}

//
// txm_free()
//
void txm_free(bool free_tex)
{
   if(free_tex)
   {
      for(size_t i = 0; i < textures->count; i++)
      {
         txm_node_t* node = (txm_node_t*)textures->collection[i];
         t_free(node->tex);
      }
   }
   list_free_elements(textures);
   list_free(textures);
}

//
// txm_push()
//
void txm_push(size_t id, size_t scope, texture_t* tex)
{
   txm_node_t* node = malloc(sizeof(txm_node_t));
   node->tex = tex;
   node->id = id;
   node->scope = scope;
   list_push(textures, node);
}

//
// txm_load_scope()
//
void txm_load_scope(size_t scope)
{
   if(!loaded_scopes[scope])
   {
      loaded_scopes[scope] = true;
      for(size_t i = 0; i < textures->count; i++)
      {
         txm_node_t* node = (txm_node_t*)textures->collection[i];
         if(node->scope == scope)
            t_load(node->tex);
      }
   }
}

//
// txm_unload_scope()
//
void txm_unload_scope(size_t scope)
{
   if(loaded_scopes[scope])
   {
      for (size_t i = 0; i < textures->count; i++)
      {
         txm_node_t* node = (txm_node_t*) textures->collection[i];
         if (node->scope == scope)
            t_unload(node->tex);
      }
      loaded_scopes[scope] = false;
   }
}

texture_t* txm_get(size_t id)
{
   for (size_t i = 0; i < textures->count; i++)
   {
      txm_node_t* node = (txm_node_t*) textures->collection[i];
      if (node->id == id)
         return node->tex;
   }

   return NULL;
}
