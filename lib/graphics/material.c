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

material_t* mt_create(char* name, uint8_t mode)
{
   material_t* material = malloc(sizeof(material_t));
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
   free(material->name);
   free(material);
}

struct _cache_node{
   float color[3];
   texture_t* texture;
};

list_t* cache;

texture_t* mt_get_from_cache(vec4 color)
{
   for(size_t i = 0; i < cache->count; i++)
   {
      struct _cache_node* node = cache->collection[i];
      if(fabsf(node->color[0] - color[0]) < .5e-3 &&
         fabsf(node->color[1] - color[1]) < .5e-3 &&
         fabsf(node->color[2] - color[2]) < .5e-3)
      {
         return node->texture;
      }
   }
}

void mt_cache(texture_t* tex, vec4 color)
{
   struct _cache_node* node = malloc(sizeof(struct _cache_node));
   node->color[0] = color[0];
   node->color[1] = color[1];
   node->color[2] = color[2];
   node->texture = tex;
}

texture_t* mt_to_texture(material_t* material, const char* field, vec4 color)
{
   texture_t* cached = mt_get_from_cache(color);
   if(cached) return cached;

   char* name = malloc(50);
   snprintf(name, 50, "__generated_mt_%s_%s_%.1f_%.1f_%.1f_",
         material->name, field, color[0], color[1], color[2]);
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

void mt_build(material_t* material)
{
   if(material->mode != MT_DEFAULT_NO_AMBIENT &&
     material->mode != MT_ILLUM &&
     material->mode != MT_ILLUM_NO_SHADOWS)
   {
     if(!material->map_ambient)
        material->map_ambient = mt_to_texture(material, "amb", material->ambient);
   }

   if(material->mode != MT_DEFAULT_NO_SPECULAR &&
      material->mode != MT_ILLUM &&
      material->mode != MT_ILLUM_NO_SHADOWS)
   {
      if(!material->map_specular)
         material->map_specular = mt_to_texture(material, "spec", material->specular);
   }

   if(!material->map_diffuse)
      material->map_diffuse = mt_to_texture(material, "diff", material->diffuse);

   if(!material->map_transparent)
      material->map_transparent = mt_to_texture(material, "trans", material->transparent);
}

void mt_init(void)
{
   cache = list_create(10);
}

void mt_fin(void)
{
   list_free_elements(cache);
   list_free(cache);
}
