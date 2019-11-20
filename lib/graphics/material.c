//
// Created by maxim on 9/12/19.
//

#ifdef __GNUC__
#pragma implementation "material.h"
#endif
#include "material.h"

material_t* mt_create(texture_t* diffuse, texture_t* specular, float shininess)
{
   material_t* material = malloc(sizeof(material_t));
   material->diffuse = diffuse;
   material->specular = specular;
   material->shininess = shininess;

   return material;
}

void mt_free(material_t* material)
{
   free(material);
}