//
// Created by maxim on 8/26/19.
//

#ifndef DEEPCARS_GRAPHICS_H
#define DEEPCARS_GRAPHICS_H

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include "gmath.h"
#include "../resources/model.h"

void gr_fill(vec4 color);

void gr_init(mat4 proj, mat4 view);
void gr_free(void);

void gr_draw_model(model_t* model);
void gr_draw_model_simpleColor(
        model_t* model, vec4 color,
        vec3f_t pos, vec3f_t scale, vec3f_t rot);

extern vec4 COLOR_WHITE;
extern vec4 COLOR_SILVER;
extern vec4 COLOR_GRAY;
extern vec4 COLOR_BLACK;
extern vec4 COLOR_RED;
extern vec4 COLOR_MAROON;
extern vec4 COLOR_YELLOW;
extern vec4 COLOR_OLIVE;
extern vec4 COLOR_LIME;
extern vec4 COLOR_GREEN;
extern vec4 COLOR_AQUA;
extern vec4 COLOR_TEAL;
extern vec4 COLOR_BLUE;
extern vec4 COLOR_NAVY;
extern vec4 COLOR_FUCHSIA;
extern vec4 COLOR_PURPLE;

#endif //DEEPCARS_GRAPHICS_H
