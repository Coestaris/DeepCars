//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_TEXTURE_H
#define DEEPCARS_TEXTURE_H

#include "../coredefs.h"

#include <stdarg.h>
#include <GL/gl.h>

#include "../../oil/oil.h"

// Storing all data required for drawing the texture
typedef struct _texture
{
   // Name of the texture
   char* name;

   // OpenGL texture ID
   GLuint texID;

   // Size of the texture
   int width;
   int height;

   // Some texture data used by OIL
   texData* data;

   // Count of texture layers
   size_t mipmaps;

   // OpenGL texture type (GL_TEXTURE_2D, GL_CUBEMAP, etc.)
   GLenum type;

} texture_t;

// texture_t constructor
texture_t* t_create(char* name);

// Frees all resources used by texture and deletes OpenGL texture
void t_free(texture_t* tex);

// Binds specified texture
void t_bind(texture_t* tex, GLenum active);

void t_set_params(texture_t* texture, GLenum target, uint32_t width, uint32_t height);

void t_set_data_png(texture_t* texture, GLenum fill_target, uint8_t* source, size_t length);

void t_set_data_dds(texture_t* texture, GLenum fill_target, uint8_t* source, size_t length);

char* t_get_pretty_signature(texture_t* t);

#endif //DEEPCARS_TEXTURE_H
