//
// Created by maxim on 9/12/19.
//

#ifndef DEEPCARS_MATERIAL_H
#define DEEPCARS_MATERIAL_H

#include "gmath.h"
#include "../resources/texture.h"

/*# Modes:
# 0 - no ambient           (ambient ignored)
# 1 - ambient              (specular ignored)
# 2 - ambient + specular
# 3 - illum                (specular igroned, ambient ignored)
# 4 - illum (no shadows)   (specular ignored, ambient ignored)
*/
typedef enum _material_type {
   MT_DEFAULT_NO_AMBIENT = 0,
   MT_DEFAULT_NO_SPECULAR = 1,
   MT_DEFAULT = 2,
   MT_ILLUM = 3,
   MT_ILLUM_NO_SHADOWS = 4,

} material_type;

typedef struct _material
{
   char* name;
   material_type mode;

   vec4 ambient;
   texture_t* map_ambient;

   vec4 diffuse;
   texture_t* map_diffuse;

   vec4 specular;
   float shininess;
   texture_t* map_specular;

   vec4 transparent;
   texture_t* map_transparent;
   texture_t* map_normal;

} material_t;

void mt_init(void);
void mt_fin(void);

material_t* mt_create(char* name, uint8_t mode);
void mt_free(material_t* material);
void mt_build(material_t* material);
texture_t* mt_create_colored_tex(vec4 color);

#endif //DEEPCARS_MATERIAL_H
