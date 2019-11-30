//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "graphics.h"
#endif
#include "graphics.h"

#define GR_LOG(format, ...) DC_LOG("graphics.c", format, __VA_ARGS__)
#define GR_ERROR(format, ...) DC_ERROR("graphics.c", format, __VA_ARGS__)

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

unsigned int depthMapFBO;
unsigned int depthMap;

#include "../camera.h"
#include "../light.h"

light_t* light;
mat4 light_space_mat;
camera_t* light_camera;

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

shader_t* shader_colored;
shader_t* shader_colored_shaded;
shader_t* shader_textured;

mat4 proj_mat;
mat4 view_mat;
mat4 model_mat;

mat4 x_rot_mat;
mat4 y_rot_mat;
mat4 z_rot_mat;

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

   shader_colored = s_get_shader(SH_COLORED);
   shader_colored_shaded = s_get_shader(SH_COLORED_SHADED);
   shader_textured = s_get_shader(SH_TEXTURED);

   proj_mat = proj;
   view_mat = view;
   model_mat = cmat4();

   x_rot_mat = cmat4();
   y_rot_mat = cmat4();
   z_rot_mat = cmat4();


   GL_CALL(glGenFramebuffers(1, &depthMapFBO));
   GL_CALL(glGenTextures(1, &depthMap));
   GL_CALL(glBindTexture(GL_TEXTURE_2D, depthMap));
   GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
           SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

   GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
   GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));
   GL_CALL(glDrawBuffer(GL_NONE));
   GL_CALL(glReadBuffer(GL_NONE));
   GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

   light = l_create(LT_DIRECTION);
   vec4_fill(light->direction, 0, -1, .3f, 0);

   float near_plane = 1.0f, far_plane = 7.5f;
   light_space_mat = cmat4();
   mat4 light_view = cmat4();

   camera_t* c = c_create(cvec4(0,0,0,0), cvec4(0,1,0,0));
   c->use_target = false;
   c->direction = light->direction;

   mat4_ortho(light_space_mat, near_plane, far_plane, 10, 10);
   c_to_mat(light_view, c);

   light_space_mat = cmat4();
   mat4_mulm(light_space_mat, light_view);

   mat4_free(light_view);

   //glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
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

   mat4_free(x_rot_mat);
   mat4_free(y_rot_mat);
   mat4_free(z_rot_mat);
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

   mat4_rotate_x(x_rot_mat, rot.x);
   mat4_rotate_y(y_rot_mat, rot.y);
   mat4_rotate_z(z_rot_mat, rot.z);

   mat4_mulm(model_mat, x_rot_mat);
   mat4_mulm(model_mat, y_rot_mat);
   mat4_mulm(model_mat, z_rot_mat);
}

void gr_draw_model_textured(model_t* model, texture_t* texture)
{
   sh_use(shader_textured);
   GLint* locations = shader_textured->uniform_locations;
   vec4 camera_pos = current_scene->camera->position;

   sh_set_int(locations[SH_TEXTURED_TEXTURE], 0);
   sh_set_vec3v(locations[SH_TEXTURED_VIEWER], camera_pos[0], camera_pos[1], camera_pos[2]);
   sh_set_mat4(locations[SH_TEXTURED_PROJ], proj_mat);
   sh_set_mat4(locations[SH_TEXTURED_VIEW], view_mat);
   sh_set_mat4(locations[SH_TEXTURED_MODEL], model_mat);

   t_bind(texture, GL_TEXTURE0);
   gr_draw_model(model);

   t_bind(NULL, GL_TEXTURE0);
   sh_use(NULL);
}

void gr_draw_model_colored_shaded(model_t* model, vec4 color, float ambient)
{
   sh_use(shader_colored_shaded);
   GLint* locations = shader_colored_shaded->uniform_locations;
   vec4 camera_pos = current_scene->camera->position;

   sh_set_vec3v(locations[SH_COLORED_SHADED_COLOR], color[0], color[1], color[2]);
   sh_set_vec3v(locations[SH_COLORED_SHADED_VIEWER], camera_pos[0], camera_pos[1], camera_pos[2]);
   sh_set_vec3v(locations[SH_COLORED_SHADED_COLOR], color[0], color[1], color[2]);
   sh_set_mat4(locations[SH_COLORED_SHADED_PROJ], proj_mat);
   sh_set_mat4(locations[SH_COLORED_SHADED_VIEW], view_mat);
   sh_set_mat4(locations[SH_COLORED_SHADED_MODEL], model_mat);
   sh_set_float(locations[SH_COLORED_SHADED_AMBIENT], ambient);

   light_t* lt = (light_t*)current_scene->lights->collection[0];

   sh_set_vec3v(locations[SH_COLORED_SHADED_L_COLOR], lt->color[0], lt->color[1], lt->color[2]);
   sh_set_vec3v(locations[SH_COLORED_SHADED_L_POS], lt->direction[1], lt->direction[2], lt->direction[3]);

   gr_draw_model(model);
   sh_use(NULL);
}

void gr_draw_model_colored(model_t* model, vec4 color)
{
   sh_use(shader_colored);
   GLint* locations = shader_colored->uniform_locations;
   vec4 camera_pos = current_scene->camera->position;

   sh_set_vec3v(locations[SH_COLORED_COLOR], color[0], color[1], color[2]);
   sh_set_vec3v(locations[SH_COLORED_COLOR], color[0], color[1], color[2]);
   sh_set_mat4(locations[SH_COLORED_PROJ], proj_mat);
   sh_set_mat4(locations[SH_COLORED_VIEW], view_mat);
   sh_set_mat4(locations[SH_COLORED_MODEL], model_mat);

   gr_draw_model(model);
   sh_use(NULL);
}