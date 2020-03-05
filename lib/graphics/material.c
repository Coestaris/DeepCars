//
// Created by maxim on 9/12/19.
//

#ifdef __GNUC__
#pragma implementation "material.h"
#endif
#include "material.h"
#include "../resources/rmanager.h"

#define MT_LOG(format, ...) DC_LOG("material.c", format, __VA_ARGS__)
#define MT_ERROR(format, ...) DC_ERROR("material.c", format, __VA_ARGS__)

struct _cache_node
{
   float color[3];
   texture_t* texture;
};

static list_t* material_cache;

material_t* mt_create(char* name, uint8_t mode)
{
   material_t* material = DEEPCARS_MALLOC(sizeof(material_t));
   material->name = name;
   material->mode = mode;

   material->ambient = cvec4(0.2, 0.2, 0.2, 0);
   material->map_ambient = NULL;

   material->diffuse = cvec4(0.8, 0.8, 0.8, 0);
   material->map_diffuse = NULL;

   material->specular = cvec4(1, 1, 1, 0);
   material->map_specular = NULL;
   material->shininess = 32;

   material->transparent = cvec4(0, 0, 0, 0);
   material->map_transparent = NULL;

   material->map_normal = NULL;
   return material;
}

void mt_free(material_t* material)
{
   vec4_free(material->ambient);
   vec4_free(material->diffuse);
   vec4_free(material->specular);
   vec4_free(material->transparent);
   MT_LOG("Material \"%s\" freed", material->name);
   DEEPCARS_FREE(material->name);
   DEEPCARS_FREE(material);
}

static texture_t* mt_get_from_cache(vec4 color)
{
   for(size_t i = 0; i < material_cache->count; i++)
   {
      struct _cache_node* node = material_cache->collection[i];
      if(fabsf(node->color[0] - color[0]) < MT_CACHE_MIN_DIST &&
         fabsf(node->color[1] - color[1]) < MT_CACHE_MIN_DIST &&
         fabsf(node->color[2] - color[2]) < MT_CACHE_MIN_DIST)
      {
         return node->texture;
      }
   }
   return NULL;
}

static void mt_cache(texture_t* tex, vec4 color)
{
   struct _cache_node* node = DEEPCARS_MALLOC(sizeof(struct _cache_node));
   node->color[0] = color[0];
   node->color[1] = color[1];
   node->color[2] = color[2];
   node->texture = tex;
   list_push(material_cache, node);
}

static texture_t* mt_to_texture(char* mat_name, const char* field, vec4 color)
{
   texture_t* cached = mt_get_from_cache(color);
   if(cached) return cached;

   char* name = DEEPCARS_MALLOC(50);
   snprintf(name, 50, "__generated_mt_%s_%s_%.1f_%.1f_%.1f",
            mat_name, field, color[0], color[1], color[2]);
   texture_t* t = t_create(name);

   GLuint id;
   GL_CALL(glGenTextures(1, &id));
   GL_CALL(glBindTexture(GL_TEXTURE_2D, id));

   const uint8_t w = 1;
   const uint8_t h = 1;
   float data[3] = {color[0], color[1], color[2]};

   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

   GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, data));

   GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
   rm_push(TEXTURE, t, -1);

   t->texID = id;
   t->width = w;
   t->height = h;
   t->type = GL_TEXTURE_2D;
   t->mipmaps = 1;

   mt_cache(t, color);
   return t;
}

texture_t* mt_create_colored_tex(vec4 color)
{
   return mt_to_texture("colored_tex", "_", color);
}

void mt_build(material_t* material)
{
   if(material->mode != MT_DEFAULT_NO_AMBIENT &&
     material->mode != MT_ILLUM &&
     material->mode != MT_ILLUM_NO_SHADOWS)
   {
     if(!material->map_ambient)
        material->map_ambient = mt_to_texture(material->name, "amb", material->ambient);
   }

   if(material->mode != MT_DEFAULT_NO_SPECULAR &&
      material->mode != MT_ILLUM &&
      material->mode != MT_ILLUM_NO_SHADOWS)
   {
      if(!material->map_specular)
         material->map_specular = mt_to_texture(material->name, "spec", material->specular);
   }

   if(!material->map_diffuse)
      material->map_diffuse = mt_to_texture(material->name, "diff", material->diffuse);

   if(!material->map_transparent)
      material->map_transparent = mt_to_texture(material->name, "trans", material->transparent);
}

void mt_init(void)
{
   material_cache = list_create();
}

void mt_release(void)
{
   list_free_elements(material_cache);
   list_free(material_cache);
}
