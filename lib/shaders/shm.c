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

// Sets precalculated uniform location end checks it to be valid
void s_set_uniform_location(size_t index, shader_t* sh, const char* uniform_name)
{
   if((sh->uniform_locations[index] = glGetUniformLocation(sh->prog_id, uniform_name)) == -1)
   {
      S_ERROR("Unable to get uniform location of \"%s\" from shader %s", uniform_name, sh->name);
   }
}

//
// s_setup_built_in_shaders
//
void s_setup_built_in_shaders()
{
   /*shader_t* colored = s_get_shader(SH_COLORED);
   colored->uniform_locations = malloc(sizeof(GLint) * 4);
   s_set_uniform_location(SH_COLORED_COLOR, colored, "objectColor");
   s_set_uniform_location(SH_COLORED_MODEL, colored, "model");
   s_set_uniform_location(SH_COLORED_VIEW, colored, "view");
   s_set_uniform_location(SH_COLORED_PROJ, colored, "projection");

   shader_t* colored_shaded = s_get_shader(SH_COLORED_SHADED);
   colored_shaded->uniform_locations = malloc(sizeof(GLint) * 8);
   s_set_uniform_location(SH_COLORED_SHADED_MODEL, colored_shaded, "model");
   s_set_uniform_location(SH_COLORED_SHADED_VIEW, colored_shaded, "view");
   s_set_uniform_location(SH_COLORED_SHADED_PROJ, colored_shaded, "projection");

   s_set_uniform_location(SH_COLORED_SHADED_COLOR, colored_shaded, "objectColor");
   s_set_uniform_location(SH_COLORED_SHADED_VIEWER, colored_shaded, "viewPos");
   s_set_uniform_location(SH_COLORED_SHADED_AMBIENT, colored_shaded, "ambient");

   s_set_uniform_location(SH_COLORED_SHADED_L_POS, colored_shaded, "lightPos");
   s_set_uniform_location(SH_COLORED_SHADED_L_COLOR, colored_shaded, "lightColor");

   shader_t* textured = s_get_shader(SH_TEXTURED);
   textured->uniform_locations = malloc(sizeof(GLint) * 5);
   s_set_uniform_location(SH_TEXTURED_TEXTURE, textured, "color_tex");
   s_set_uniform_location(SH_TEXTURED_MODEL, textured, "model");
   s_set_uniform_location(SH_TEXTURED_VIEW, textured, "view");
   s_set_uniform_location(SH_TEXTURED_PROJ, textured, "projection");
   s_set_uniform_location(SH_TEXTURED_VIEWER, textured, "viewerPosition");*/
}

//
// s_init
//
void s_init()
{
   nodes = list_create(10);
}

//
// s_get_shader
//
shader_t* s_get_shader(int id)
{
   for (size_t i = 0; i < nodes->count; i++)
      if (((shm_node_t*)nodes->collection[i])->id == id)
         return ((shm_node_t*)nodes->collection[i])->shader;

   return NULL;
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