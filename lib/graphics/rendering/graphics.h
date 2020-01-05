//
// Created by maxim on 8/26/19.
//

#ifndef DEEPCARS_GRAPHICS_H
#define DEEPCARS_GRAPHICS_H

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include "../gmath.h"
#include "../../resources/model.h"
#include "../../resources/shader.h"
#include "../../resources/shm.h"
#include "../../object.h"
#include "render_stage.h"

void gr_fill(vec4 color);

void gr_init(void);
void gr_free(void);

mat4 gr_transform(vec3f_t pos, vec3f_t scale, vec3f_t rot);

//void gr_draw_model_colored(model_t* model, vec4 color);
//void gr_draw_model_colored_shaded(model_t* model, vec4 color, float ambient);
//void gr_draw_model_textured(model_t* model, texture_t* texture);
void gr_render_object(object_t* obj);
void gr_render_vao(GLuint vao);
void gr_bind(render_stage_t* stage);
void gr_unbind(render_stage_t* stage);

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

extern mat4 proj_mat;
extern mat4 view_mat;
extern mat4 model_mat;

#endif //DEEPCARS_GRAPHICS_H
