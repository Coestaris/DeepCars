//
// Created by maxim on 4/29/19.
//

#ifdef __GNUC__
#pragma implementation "shm.h"
#endif
#include "shm.h"

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

// Sets precalculated uniform location end checks it to be valid
void s_set_uniform_location(size_t index, shader_t* sh, const char* uniform_name)
{
   if((sh->uniform_locations[index] = glGetUniformLocation(sh->prog_id, uniform_name)) == -1)
   {
      printf("[shm.c]: Unable to get uniform location \"%s\" from shader %s\n", uniform_name, sh->fragment_path);
      //exit(EXIT_FAILURE);
   }
}

//
// s_push_built_in_shaders
//
void s_push_built_in_shaders()
{
   shader_t* simple_colored = sh_create("../lib/shaders/shaders/simple_colored.vsh",
                                        "../lib/shaders/shaders/simple_colored.fsh");
   sh_info(simple_colored);

   simple_colored->uniform_locations = malloc(sizeof(GLint) * 5);
   s_set_uniform_location(SH_SIMPLECOLORED_COLOR, simple_colored, "objectColor");
   s_set_uniform_location(SH_SIMPLECOLORED_MODEL, simple_colored, "model");
   s_set_uniform_location(SH_SIMPLECOLORED_VIEW, simple_colored, "view");
   s_set_uniform_location(SH_SIMPLECOLORED_PROJ, simple_colored, "projection");
   s_set_uniform_location(SH_SIMPLECOLORED_VIEWER, simple_colored, "viewerPosition");
   s_push(simple_colored, SH_SIMPLECOLORED);
}

//
// s_init
//
void s_init()
{
   nodes = list_create(10);
   s_push_built_in_shaders();
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