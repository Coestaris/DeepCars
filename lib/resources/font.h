//
// Created by maxim on 1/16/20.
//

#ifndef DEEPCARS_FONT_H
#define DEEPCARS_FONT_H

#include "../coredefs.h"

#include "shader.h"
#include "../graphics/win.h"

// Proportional downscale of all font metric parameters
#define FONT_SCALE 8.0f

// Stores all the information needed to draw a character
typedef struct _charinfo
{
   // Code of the char
   int id;

   // Precalculated texture coordinates in atlas
   float tex_coord[8];

   // Total size of the character
   float width;
   float height;

   // The value added to the cursor x to get the real offset coordinate
   float yoffset;
   // The value added to the cursor y to get the real offset coordinate
   float xoffset;

   // The value added to cursor x to get position of next character
   float xadvance;

} charinfo_t;

// Describes font for rendering text
typedef struct _font
{
   // Font name
   char* name;

   // Function that will be called when drawing text
   void (*bind_func)(struct _font* font, void* data);

   // Shader program
   shader_t* shader;
   // Font atlas
   texture_t* texture;

   // Count of chars in font
   size_t chars;
   // Height of chars in font
   float base;

   // Information about all chars
   charinfo_t infos[256];

   // OpenGL buffers for rendering text
   GLuint vao;
   GLuint vbo;

   // Current window
   win_info_t* win;

} font_t;

// font_t constructor
font_t* f_create(char* name, texture_t* texture, shader_t* shader, uint8_t* info, size_t infolen);

// Frees font
void f_free(font_t* font);

#endif //DEEPCARS_FONT_H
