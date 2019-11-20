//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_TEXTURE_H
#define DEEPCARS_TEXTURE_H

#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>

#include <GL/gl.h>

#include "../../oil/oil.h"
#include "../structs.h"

// Storing all data required for drawing the texture
typedef struct _texture
{
   // Path to the texture file
   const char* fn;

   // OpenGL texture ID
   GLuint texID;

   // Size of the texture
   int width;
   int height;

   // Some texture data used by OIL
   texData* data;

} texture_t;

// texture_t constructor
texture_t* t_create(const char* fn);

// Frees all resources used by texture and deletes OpenGL texture
void t_free(texture_t* tex);

// Deletes OpenGL texture
void t_unload(texture_t* tex);

// Loads texture from specified file
void t_load(texture_t* tex);

// Binds specified texture
void t_bind(texture_t* tex);

#endif //DEEPCARS_TEXTURE_H
