//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "graphics.h"
#endif
#include "graphics.h"

// for current scene
#include "../scm.h"

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
   GL_PCALL(glClearColor(color[0], color[1], color[2], color[3]))
}

shader_t* shader_simple;
shader_t* shader_textured;

mat4 proj_mat;
mat4 view_mat;
mat4 model_mat;

void gr_init(mat4 proj, mat4 view)
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

   shader_simple = s_getShader(SH_SIMPLECOLORED);
   shader_textured = s_getShader(SH_TEXTURED);

   proj_mat = proj;
   view_mat = view;
   model_mat = cmat4();

   //GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE))
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

   mat4_free(proj_mat);
   mat4_free(view_mat);
   mat4_free(model_mat);
}

void gr_draw_model(model_t* model)
{
   GL_PCALL(glBindVertexArray(model->VAO))
   GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, model->triangles_count * 3))
   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, 0))
   GL_PCALL(glBindVertexArray(0))
}

inline void gr_transform(vec3f_t pos, vec3f_t scale, vec3f_t rot)
{
   mat4_identity(model_mat);
   mat4_translate(model_mat, (float) pos.x, (float) pos.y, (float) pos.z);
   mat4_scale(model_mat, (float) scale.x, (float) scale.y, (float) scale.z);

   //todo: rotation
}

void gr_draw_model_textured(model_t* model, texture_t* texture)
{
   sh_use(shader_textured);
   GLint* locations = shader_textured->uniform_locations;
   vec4 camera_pos = current_scene->camera->position;

   sh_set_int(shader_textured, locations[SH_TEXTURED_TEXTURE], 0);
   sh_set_vec3v(shader_textured, locations[SH_TEXTURED_VIEWER],
                camera_pos[0], camera_pos[1], camera_pos[2]);

   sh_set_mat4(shader_textured, locations[SH_TEXTURED_PROJ], proj_mat);
   sh_set_mat4(shader_textured, locations[SH_TEXTURED_VIEW], view_mat);
   sh_set_mat4(shader_textured, locations[SH_TEXTURED_MODEL], model_mat);


   t_bind(texture, GL_TEXTURE0);
   gr_draw_model(model);

   t_bind(NULL, GL_TEXTURE0);
   sh_use(NULL);
}

void gr_draw_model_simple(model_t* model, vec4 color)
{
   sh_use(shader_simple);
   GLint* locations = shader_simple->uniform_locations;
   vec4 camera_pos = current_scene->camera->position;

   sh_set_vec3v(shader_simple, locations[SH_SIMPLECOLORED_COLOR],
                color[0], color[1], color[2]);
   sh_set_vec3v(shader_simple, locations[SH_SIMPLECOLORED_VIEWER],
                camera_pos[0], camera_pos[1], camera_pos[2]);
   sh_set_vec3v(shader_simple, locations[SH_SIMPLECOLORED_COLOR],
                color[0], color[1], color[2]);
   sh_set_mat4(shader_simple, locations[SH_SIMPLECOLORED_PROJ], proj_mat);
   sh_set_mat4(shader_simple, locations[SH_SIMPLECOLORED_VIEW], view_mat);
   sh_set_mat4(shader_simple, locations[SH_SIMPLECOLORED_MODEL], model_mat);

   gr_draw_model(model);
   sh_use(NULL);
}