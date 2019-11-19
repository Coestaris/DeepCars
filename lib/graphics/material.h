//
// Created by maxim on 9/12/19.
//

#ifndef DEEPCARS_MATERIAL_H
#define DEEPCARS_MATERIAL_H

#include "gmath.h"
#include "../resources/texture.h"

typedef struct _material
{
   texture_t* diffuse;
   texture_t* specular;

   float shininess;

} material_t;

material_t* mt_create(texture_t* diffuse, texture_t* specular, float shininess);
void mt_free(material_t* material);

#endif //DEEPCARS_MATERIAL_H
