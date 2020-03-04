//
// Created by maxim on 1/22/20.
//

#ifndef DEEPCARS_TEXT_RENDERING_H
#define DEEPCARS_TEXT_RENDERING_H

#include <stdint.h>
#include "../../lib/graphics/gmath.h"
#include "../../lib/structs.h"
#include "../../lib/graphics/win.h"
extern mat4 primitive_proj;

typedef struct _font_data {
   vec4 color;
   vec4 border_color;
   float color_off;
   float color_k;
   float back_off;
   float back_k;

} font_data_t;

void draw_default_string(uint8_t depth, font_data_t* data, vec2 pos, vec2 scale, char* str);
void draw_monospace_string(uint8_t depth, font_data_t* data, vec2 pos, vec2 scale, char* str);
void init_fonts(win_info_t* info);

#endif //DEEPCARS_TEXT_RENDERING_H
