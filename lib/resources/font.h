//
// Created by maxim on 1/16/20.
//

#ifndef DEEPCARS_FONT_H
#define DEEPCARS_FONT_H

#include "../coredefs.h"

#include "shader.h"
#include "../graphics/win.h"

#define FONT_SCALE 8.0f

typedef struct _charinfo {
   int id;

   float tex_coord[8];

   float width;
   float height;

   float yoffset;
   float xoffset;
   float xadvance;

} charinfo_t;

typedef struct _font {
   char* name;

   void (*bind_func)(struct _font* font, void* data);

   shader_t* shader;
   texture_t* texture;

   size_t chars;
   float line_height;
   float base;

   win_info_t* win;
   charinfo_t infos[256];

   GLuint vao;
   GLuint vbo;

} font_t;

font_t* f_create(char* name, texture_t* texture, shader_t* shader, uint8_t* info, size_t infolen);
void f_free(font_t* font);

#endif //DEEPCARS_FONT_H
