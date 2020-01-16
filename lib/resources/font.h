//
// Created by maxim on 1/16/20.
//

#ifndef DEEPCARS_FONT_H
#define DEEPCARS_FONT_H

#include "shader.h"

#define FONT_SCALE 8.0f

typedef struct _charinfo {
   float tex_coord[8];
   float offset[2];

   float width;

} charinfo_t;

typedef struct _font {
   char* name;

   shader_t* shader;
   texture_t* texture;

   size_t chars;
   float line_height;
   float base;

   charinfo_t infos[256];

} font_t;

font_t* f_create(char* name, texture_t* texture, shader_t* shader, uint8_t* info, size_t infolen);
void f_free(font_t* font);

#endif //DEEPCARS_FONT_H
