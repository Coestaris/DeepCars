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
list_t* shader_nodes;

//
// s_has_shader
//
bool s_has_shader(int id)
{
   for (size_t i = 0; i < shader_nodes->count; i++)
      if (((shm_node_t*)shader_nodes->collection[i])->id == id) return true;
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

   list_push(shader_nodes, node);
}

//
// s_init
//
void s_init()
{
   shader_nodes = list_create();
}

//
// s_getn_shader
//
shader_t* s_getn_shader(const char* name)
{
   for (size_t i = 0; i < shader_nodes->count; i++)
      if (!strcmp(((shm_node_t*)shader_nodes->collection[i])->shader->name, name))
         return ((shm_node_t*)shader_nodes->collection[i])->shader;

   S_ERROR("Unable to find shader with name \"%s\"", name);
   return NULL; // stub
}

//
// s_get_shader
//
shader_t* s_get_shader(int id)
{
   for (size_t i = 0; i < shader_nodes->count; i++)
      if (((shm_node_t*)shader_nodes->collection[i])->id == id)
         return ((shm_node_t*)shader_nodes->collection[i])->shader;

   S_ERROR("Unable to find shader with id \"%i\"", id);
   return NULL; // stub
}

//
// s_free
//
void s_free(bool free_shaders)
{
   if(free_shaders)
   {
      for (size_t i = 0; i < shader_nodes->count; i++)
         sh_free(((shm_node_t*) shader_nodes->collection[i])->shader);
   }

   list_free_elements(shader_nodes);
   list_free(shader_nodes);
}