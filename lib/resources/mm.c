//
// Created by maxim on 11/28/19.
//

#ifdef __GNUC__
#pragma implementation "mm.h"
#endif
#include "mm.h"

#define MM_LOG(format, ...) DC_LOG("mm.c", format, __VA_ARGS__)
#define MM_ERROR(format, ...) DC_ERROR("mm.c", format, __VA_ARGS__)

#include <string.h>

// Contains data necessary for indexing models in a list
typedef struct _mm_node {
   size_t id;
   model_t* model;

} mm_node_t;

// All models stored in a mm
list_t* models;

//
// mm_init()
//
void mm_init(void)
{
   models = list_create(10);
}

//
// mm_free()
//
void mm_free(bool free_models)
{
   if(free_models)
   {
      for(size_t i = 0; i < models->count; i++)
      {
         mm_node_t* node = (mm_node_t*)models->collection[i];
         m_free(node->model);
      }
   }

   list_free_elements(models);
   list_free(models);
}

//
// mm_get()
//
model_t* mm_get(size_t id)
{
   for (size_t i = 0; i < models->count; i++)
   {
      mm_node_t* node = (mm_node_t*)models->collection[i];
      if (node->id == id)
         return node->model;
   }

   return NULL;
}

//
// mm_push()
//
void mm_push(size_t id, model_t* model, bool build)
{
   mm_node_t* node = malloc(sizeof(mm_node_t));
   if(build) m_build(model);

   node->model = model;
   node->id = id;
   list_push(models, node);
}