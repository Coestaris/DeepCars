//
// Created by maxim on 11/23/19.
//

#ifdef __GNUC__
#pragma implementation "txm.h"
#endif
#include "txm.h"

#define TXM_LOG(format, ...) DC_LOG("txm.c", format, __VA_ARGS__)
#define TXM_ERROR(format, ...) DC_ERROR("txm.c", format, __VA_ARGS__)

#include <string.h>
#include "../structs.h"

// Contains data necessary for indexing textures in a list
typedef struct _txm_node {
   size_t id;
   texture_t* tex;

} txm_node_t;

// All textures stored in a txm
list_t* textures;

//
// txm_init()
//
void txm_init()
{
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
void txm_push(size_t id, texture_t* tex)
{
   txm_node_t* node = malloc(sizeof(txm_node_t));
   node->tex = tex;
   node->id = id;
   list_push(textures, node);
}

//
// txm_get()
//
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
