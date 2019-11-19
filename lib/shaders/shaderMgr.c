//
// Created by maxim on 4/29/19.
//

#ifdef __GNUC__
#pragma implementation "shaderMgr.h"
#endif
#include "shaderMgr.h"

// Soring shader and its id
typedef struct _shm_node
{
   shader_t* shader;
   int id;
} shm_node_t;

// Global list of manager's shaders
list_t* nodes;

//
// s_hasShader
//
bool s_hasShader(int id)
{
   for (size_t i = 0; i < nodes->count; i++)
      if (((shm_node_t*)nodes->collection[i])->id == id) return 1;
   return 0;
}

//
// s_push
//
void s_push(shader_t* shader, int id)
{
   assert(shader != NULL);
   assert(!s_hasShader(id));

   shm_node_t* node = malloc(sizeof(shm_node_t));
   node->shader = shader;
   node->id = id;

   list_push(nodes, node);
}

//
// s_pushBuiltInShaders
//
void s_pushBuiltInShaders()
{
   shader_t* simpleColored = sh_create("../lib/shaders/shaders/simpleColored.vsh",
                                       "../lib/shaders/shaders/simpleColored.fsh");
   sh_info(simpleColored);

   simpleColored->uniform_locations = malloc(sizeof(GLint) * 4);
   simpleColored->uniform_locations[SH_SIMPLECOLORED_COLOR] = glGetUniformLocation(simpleColored->prog_id, "objectColor");
   simpleColored->uniform_locations[SH_SIMPLECOLORED_MODEL] = glGetUniformLocation(simpleColored->prog_id, "model");
   simpleColored->uniform_locations[SH_SIMPLECOLORED_VIEW] = glGetUniformLocation(simpleColored->prog_id, "view");
   simpleColored->uniform_locations[SH_SIMPLECOLORED_PROJ] = glGetUniformLocation(simpleColored->prog_id, "projection");

   s_push(simpleColored, SH_SIMPLECOLORED);
}

//
// s_init
//
void s_init()
{
   nodes = list_create(10);
   s_pushBuiltInShaders();
}

//
// s_getShader
//
shader_t* s_getShader(int id)
{
   for (size_t i = 0; i < nodes->count; i++)
      if (((shm_node_t*)nodes->collection[i])->id == id)
         return ((shm_node_t*)nodes->collection[i])->shader;

   return NULL;
}

//
// s_free
//
void s_free(void)
{
   for (size_t i = 0; i < nodes->count; i++)
      sh_free(((shm_node_t*)nodes->collection[i])->shader);

   list_free_elements(nodes);
   list_free(nodes);
}