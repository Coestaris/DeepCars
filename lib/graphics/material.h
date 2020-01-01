//
// Created by maxim on 9/12/19.
//

#ifndef DEEPCARS_MATERIAL_H
#define DEEPCARS_MATERIAL_H

#include "gmath.h"
#include "../resources/texture.h"

typedef struct _material
{
   int mode;

   vec4 ambient;
   texture_t* map_ambient;

   vec4 diffuse;
   texture_t* map_diffuse;

   vec4 specular;
   float shininess;
   texture_t* map_specular;

   float transparent;
   texture_t* map_transparent;
   texture_t* map_normal;

} material_t;

material_t* mt_create();
void mt_free(material_t* material);

#endif //DEEPCARS_MATERIAL_H
