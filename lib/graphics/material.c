//
// Created by maxim on 9/12/19.
//

#ifdef __GNUC__
#pragma implementation "material.h"
#endif
#include "material.h"

#define MT_LOG(format, ...) DC_LOG("material.c", format, __VA_ARGS__)
#define MT_ERROR(format, ...) DC_ERROR("material.c", format, __VA_ARGS__)

material_t* mt_create()
{
   material_t* material = malloc(sizeof(material_t));
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
   free(material);
}