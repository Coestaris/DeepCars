//
// Created by maxim on 9/12/19.
//

#ifndef ZOMBOID3D_MATERIAL_H
#define ZOMBOID3D_MATERIAL_H

#include "gmath.h"
#include "../textures/texture.h"

typedef struct _material {
    texture_t* diffuse;
    texture_t* specular;

    float shininess;

} material_t;

material_t* mt_create(texture_t* diffuse, texture_t* specular, float shininess);
void mt_free(material_t* material);

#endif //ZOMBOID3D_MATERIAL_H
