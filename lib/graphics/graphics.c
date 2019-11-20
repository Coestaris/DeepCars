//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "graphics.h"
#endif
#include "graphics.h"

vec4 COLOR_WHITE;
vec4 COLOR_SILVER;
vec4 COLOR_GRAY;
vec4 COLOR_BLACK;
vec4 COLOR_RED;
vec4 COLOR_MAROON;
vec4 COLOR_YELLOW;
vec4 COLOR_OLIVE;
vec4 COLOR_LIME;
vec4 COLOR_GREEN;
vec4 COLOR_AQUA;
vec4 COLOR_TEAL;
vec4 COLOR_BLUE;
vec4 COLOR_NAVY;
vec4 COLOR_FUCHSIA;
vec4 COLOR_PURPLE;

void gr_fill(vec4 color)
{
   glClearColor(color[0], color[1], color[2], color[3]);
}

shader_t* shader_simpleColored;
mat4 projMat;
mat4 view_mat;
mat4 modelMat;

void gr_init(mat4 _proj, mat4 _view)
{
   COLOR_WHITE = cvec4(1, 1, 1, 0);
   COLOR_SILVER = cvec4(.75, .75, .75, 0);
   COLOR_GRAY = cvec4(.5, .5, .5, 0);
   COLOR_BLACK = cvec4(0, 0, 0, 0);
   COLOR_RED = cvec4(1, 0, 0, 0);
   COLOR_MAROON = cvec4(.5, 0, 0, 0);
   COLOR_YELLOW = cvec4(1, 1, 0, 0);
   COLOR_OLIVE = cvec4(.5, .5, 0, 0);
   COLOR_LIME = cvec4(0, 1, 0, 0);
   COLOR_GREEN = cvec4(0, 0.5, 0, 0);
   COLOR_AQUA = cvec4(0, 1, 1, 0);
   COLOR_TEAL = cvec4(0, .5, .5, 0);
   COLOR_BLUE = cvec4(0, 0, 1, 0);
   COLOR_NAVY = cvec4(0, 0, .5, 0);
   COLOR_FUCHSIA = cvec4(1, 0, 1, 0);
   COLOR_PURPLE = cvec4(.5, 0, .5, 0);

   shader_simpleColored = s_getShader(SH_SIMPLECOLORED);

   projMat = _proj;
   view_mat = _view;
   modelMat = cmat4();
}

void gr_free(void)
{
   vec4_free(COLOR_WHITE);
   vec4_free(COLOR_SILVER);
   vec4_free(COLOR_GRAY);
   vec4_free(COLOR_BLACK);
   vec4_free(COLOR_RED);
   vec4_free(COLOR_MAROON);
   vec4_free(COLOR_YELLOW);
   vec4_free(COLOR_OLIVE);
   vec4_free(COLOR_LIME);
   vec4_free(COLOR_GREEN);
   vec4_free(COLOR_AQUA);
   vec4_free(COLOR_TEAL);
   vec4_free(COLOR_BLUE);
   vec4_free(COLOR_NAVY);
   vec4_free(COLOR_FUCHSIA);
   vec4_free(COLOR_PURPLE);

   mat4_free(projMat);
   mat4_free(view_mat);
   mat4_free(modelMat);
}

void gr_draw_model(model_t* model)
{
   glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
   glBindVertexArray(model->VAO);

   glDrawArrays(GL_TRIANGLES, 0, model->model_len->faces_count * 3);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

void gr_draw_model_simpleColor(
        model_t* model, vec4 color,
        vec3f_t pos, vec3f_t scale, vec3f_t rot)
{
   sh_use(shader_simpleColored);
   sh_set_vec3v(shader_simpleColored,
                shader_simpleColored->uniform_locations[SH_SIMPLECOLORED_COLOR],
                color[0], color[1], color[2]);

   sh_set_mat4(shader_simpleColored,
               shader_simpleColored->uniform_locations[SH_SIMPLECOLORED_PROJ],
               projMat);

   sh_set_mat4(shader_simpleColored,
               shader_simpleColored->uniform_locations[SH_SIMPLECOLORED_VIEW],
               view_mat);

   mat4_identity(modelMat);
   mat4_translate(modelMat, (float) pos.x, (float) pos.y, (float) pos.z);
   mat4_scale(modelMat, (float) scale.x, (float) scale.y, (float) scale.z);

   sh_set_mat4(shader_simpleColored,
               shader_simpleColored->uniform_locations[SH_SIMPLECOLORED_MODEL],
               modelMat);

   gr_draw_model(model);
   sh_use(NULL);
}