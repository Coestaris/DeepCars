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
   // Name of the texture
   const char* name;

   // OpenGL texture ID
   GLuint texID;

   // Size of the texture
   int width;
   int height;

   // Some texture data used by OIL
   texData* data;

} texture_t;

// texture_t constructor
texture_t* t_create(char* name);

// Frees all resources used by texture and deletes OpenGL texture
void t_free(texture_t* tex);

// Loads texture from source buffer. If png == false using dds format
void t_load_s(texture_t* tex, uint8_t* source, size_t length, bool png);

// Loads texture from specified file. If png == false using dds format
void t_load(texture_t* tex, const char* path, bool png);

// Binds specified texture
void t_bind(texture_t* tex, GLenum target);


#endif //DEEPCARS_TEXTURE_H
