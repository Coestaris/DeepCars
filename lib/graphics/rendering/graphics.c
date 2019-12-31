//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "graphics.h"
#endif
#include "graphics.h"

#define GR_LOG(format, ...) DC_LOG("graphics.c", format, __VA_ARGS__)
#define GR_ERROR(format, ...) DC_ERROR("graphics.c", format, __VA_ARGS__)

#include "../light.h"

// for current scene
#include "../../scm.h"

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

mat4 proj_mat;
mat4 view_mat;
mat4 model_mat;

mat4 x_rot_mat;
mat4 y_rot_mat;
mat4 z_rot_mat;

void gr_init()
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

   proj_mat = cmat4();
   view_mat = cmat4();
   model_mat = cmat4();

   x_rot_mat = cmat4();
   y_rot_mat = cmat4();
   z_rot_mat = cmat4();

   //GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE))
}

void gr_render_vao(GLuint vao)
{
   GL_PCALL(glBindVertexArray(vao));
   //GL_PCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

   GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

   GL_PCALL(glBindVertexArray(0));
   //GL_PCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
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

   mat4_free(x_rot_mat);
   mat4_free(y_rot_mat);
   mat4_free(z_rot_mat);
}

void gr_draw_model(model_t* model)
{
   GL_PCALL(glBindVertexArray(model->VAO))
   GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, model->triangles_count * 3))
   //GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, 0))
   GL_PCALL(glBindVertexArray(0))
}

inline mat4 gr_transform(vec3f_t pos, vec3f_t scale, vec3f_t rot)
{
   mat4_identity(model_mat);
   mat4_translate(model_mat, (float) pos.x, (float) pos.y, (float) pos.z);
   mat4_scale(model_mat, (float) scale.x, (float) scale.y, (float) scale.z);

   mat4_rotate_x(x_rot_mat, rot.x);
   mat4_rotate_y(y_rot_mat, rot.y);
   mat4_rotate_z(z_rot_mat, rot.z);

   mat4_mulm(model_mat, x_rot_mat);
   mat4_mulm(model_mat, y_rot_mat);
   mat4_mulm(model_mat, z_rot_mat);

   return model_mat;
}

void gr_render_object(object_t* obj)
{
   gr_draw_model(obj->draw_info->model);
}

void gr_bind(render_stage_t* stage)
{
   GL_PCALL(glViewport(0, 0, stage->tex_width, stage->tex_height))
   if(stage->render_mode != RM_BYPASS && stage->render_mode != RM_CUSTOM)
   {
      GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, stage->fbo));
   }
}

void gr_unbind(render_stage_t* stage)
{
   if(stage->render_mode != RM_BYPASS && stage->render_mode != RM_CUSTOM)
   {
      GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }
}