//
// Created by maxim on 4/29/19.
//

#ifdef __GNUC__
#pragma implementation "shm.h"
#endif
#include "shm.h"

#define S_LOG(format, ...) DC_LOG("shm.c", format, __VA_ARGS__)
#define S_ERROR(format, ...) DC_ERROR("shm.c", format, __VA_ARGS__)

// Soring shader and its id
typedef struct _shm_node
{
   shader_t* shader;
   int id;
} shm_node_t;

// Global list of manager's shaders
list_t* nodes;

//
// s_has_shader
//
bool s_has_shader(int id)
{
   for (size_t i = 0; i < nodes->count; i++)
      if (((shm_node_t*)nodes->collection[i])->id == id) return true;
   return false;
}

//
// s_push
//
void s_push(shader_t* shader, int id)
{
   assert(shader != NULL);
   if(s_has_shader(id))
   {
      S_ERROR("Shader with same id already exists",0);
   }

   shm_node_t* node = malloc(sizeof(shm_node_t));
   node->shader = shader;
   node->id = id;

   list_push(nodes, node);
}

//
// s_init
//
void s_init()
{
   nodes = list_create();
}

//
// s_getn_shader
//
shader_t* s_getn_shader(const char* name)
{
   for (size_t i = 0; i < nodes->count; i++)
      if (!strcmp(((shm_node_t*)nodes->collection[i])->shader->name, name))
         return ((shm_node_t*)nodes->collection[i])->shader;

   S_ERROR("Unable to find shader with name \"%s\"", name)
}

//
// s_get_shader
//
shader_t* s_get_shader(int id)
{
   for (size_t i = 0; i < nodes->count; i++)
      if (((shm_node_t*)nodes->collection[i])->id == id)
         return ((shm_node_t*)nodes->collection[i])->shader;

   S_ERROR("Unable to find shader with id \"%i\"", id)
}

//
// s_free
//
void s_free(bool free_shaders)
{
   if(free_shaders)
   {
      for (size_t i = 0; i < nodes->count; i++)
         sh_free(((shm_node_t*) nodes->collection[i])->shader);
   }

   list_free_elements(nodes);
   list_free(nodes);
}