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
   material->map_specular = NULL;

   material->specular = cvec4(1, 1, 1, 0);
   material->map_specular = NULL;
   material->shininess = 32;

   material->transparent = 0;
   material->map_transparent = NULL;

   material->map_normal = NULL;
   return material;
}

void mt_free(material_t* material)
{
   vec4_free(material->ambient);
   vec4_free(material->diffuse);
   vec4_free(material->specular);
   MT_LOG("Material \"%s\" freed", material->name);
   free(material->name);
   free(material);
}

texture_t* mt_to_texture(material_t* material, vec4 color)
{
   char* name = malloc(40);
   snprintf(name, 40, "__generated_mt_%s_%.1f_%.1f_%.1f_",
         material->name, color[0], color[1], color[2]);
   texture_t* t = t_create(name);

   GLuint id;
   glGenTextures(1, &id);

   glBindTexture(GL_TEXTURE_2D, id);



   glBindTexture(GL_TEXTURE_2D, 0);

   rm_push(TEXTURE, t, -1);
}

void mt_build(material_t* material)
{
   if(material->mode != MT_DEFAULT_NO_AMBIENT &&
     material->mode != MT_ILLUM &&
     material->mode != MT_ILLUM_NO_SHADOWS)
   {
     if(!material->map_ambient)
        material->map_ambient = mt_to_texture(material, material->ambient);
   }

   if(material->mode != MT_DEFAULT_NO_SPECULAR &&
      material->mode != MT_ILLUM &&
      material->mode != MT_ILLUM_NO_SHADOWS)
   {
      if(!material->map_specular)
         material->map_specular = mt_to_texture(material, material->specular);
   }

   if(!material->map_diffuse)
      material->map_diffuse = mt_to_texture(material, material->ambient);
}