//
// Created by maxim on 9/12/19.
//

#ifndef DEEPCARS_MATERIAL_H
#define DEEPCARS_MATERIAL_H

#include "../coredefs.h"

#include "gmath.h"
#include "../resources/texture.h"

#define MT_CACHE_MIN_DIST 1.e-4

// Describes material rendering type. Modes:
// 0 - no ambient           (ambient ignored)
// 1 - ambient              (specular ignored)
// 2 - ambient + specular
// 3 - illum                (specular igroned, ambient ignored)
// 4 - illum (no shadows)   (specular ignored, ambient ignored)
typedef enum _material_type {
   MT_DEFAULT_NO_AMBIENT = 0,
   MT_DEFAULT_NO_SPECULAR = 1,
   MT_DEFAULT = 2,
   MT_ILLUM = 3,
   MT_ILLUM_NO_SHADOWS = 4,

} material_type;

// Describes rendering materials
typedef struct _material
{
   // Name of the material
   char* name;
   // Rendering mode
   material_type mode;

   // Ambient color (used if no texture specified)
   vec4 ambient;
   // Ambient texture
   texture_t* map_ambient;

   // Diffuse (color) color (used if no texture specified)
   vec4 diffuse;
   // Color texture
   texture_t* map_diffuse;

   // Specular color (used if no texture specified)
   vec4 specular;
   // Specular texture
   texture_t* map_specular;
   // Shininess exponent multiplier
   float shininess;

   // Transparent color (used if no texture specified)
   vec4 transparent;
   // Transparent texture
   texture_t* map_transparent;

   // Normal map (ignored if set to NULL)
   texture_t* map_normal;

} material_t;

// Allocates all necessary global resources for material library
void mt_init(void);

// Releases all global resources used by materials
void mt_release(void);

// material_t constructor
material_t* mt_create(char* name, uint8_t mode);

// Frees material
void mt_free(material_t* material);

// Creates OpenGL textures depending on material type
void mt_build(material_t* material);

// Create 1x1 texture filled with specified color
texture_t* mt_create_colored_tex(vec4 color);

#endif //DEEPCARS_MATERIAL_H
