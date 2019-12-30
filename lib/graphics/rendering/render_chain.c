//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_chain.h"
#endif
#include "render_chain.h"
#include "../win.h"
#include "../../resources/txm.h"

render_chain_t* current_chain;

render_chain_t* rc_create()
{
   render_chain_t* this = malloc(sizeof(render_chain_t));
   this->data = NULL;
   this->stages = list_create(10);
   return this;
}

void create_perspective(win_info_t* win, mat4 mat, float fov, float near, float far)
{
   mat4_perspective_fov(mat, fov, (float)win->w / (float)win->h, near, far);
}

void create_ortho(win_info_t* win, mat4 mat, float near, float far)
{
   mat4_ortho(mat, near, far, win->w, win->h);
}

void rc_set_current(render_chain_t* rc)
{
   current_chain = rc;
}

render_chain_t* rc_get_current(void)
{
   return current_chain;
}

void bind_bypass(render_stage_t* stage)
{
   sh_nset_int(stage->shader, "tex", 0);
   sh_nset_mat4(stage->shader, "proj", stage->proj);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, stage->prev_stage->color_tex);
}

void unbind_bypass(render_stage_t* stage)
{
   t_bind(NULL, 0, GL_TEXTURE_2D);
}


void unbind_default(render_stage_t* stage)
{

}

typedef struct _default_shader_data {
   camera_t* camera;
   mat4 buffmat;

} default_shader_data_t;

void bind_default(render_stage_t* stage)
{
   default_shader_data_t* data = (default_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_mat4(stage->shader, "view", data->buffmat);

   glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
   glClear(GL_DEPTH_BUFFER_BIT);
}
void setup_default(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_nset_vec3v(stage->shader, "objectColor",
           object->draw_info->object_color[0],
           object->draw_info->object_color[1],
           object->draw_info->object_color[2]);
   sh_nset_mat4(stage->shader, "model", model_mat);
}

void bind_skybox(render_stage_t* stage)
{
   default_shader_data_t* data = (default_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   data->buffmat[3] = 0;
   data->buffmat[7] = 0;
   data->buffmat[11] = 0;
   data->buffmat[15] = 0;

   sh_nset_mat4(stage->shader, "view", data->buffmat);
   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_int(stage->shader, "skybox", 0);

   GL_PCALL(glActiveTexture(GL_TEXTURE0));
   t_bind(txm_get(10), 0, GL_TEXTURE_CUBE_MAP);
   // skybox cube
}

void unbind_skybox(render_stage_t* stage)
{
   t_bind(NULL, 0, GL_TEXTURE_CUBE_MAP);
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void draw_skybox(render_stage_t* stage)
{
   stage->color_tex = stage->prev_stage->color_tex;
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, stage->prev_stage->fbo));

   GL_PCALL(glEnable(GL_DEPTH_TEST));
   GL_PCALL(glDepthFunc(GL_LEQUAL));  // change depth_tex function so depth_tex test passes when values are equal to depth_tex buffer's content

   GL_PCALL(glBindVertexArray(stage->vao));
   GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, 36));

   GL_PCALL(glBindVertexArray(0));
   GL_PCALL(glDepthFunc(GL_LESS)); // set depth_tex function back to default
   GL_PCALL(glDisable(GL_DEPTH_TEST));

   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

GLuint quad_vao;
GLuint quad_vbo;
GLuint quad_ebo;

GLuint cube_vao;
GLuint cube_vbo;

void setup_cube()
{
   float skyboxVertices[] = {
           // positions
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f,  1.0f
   };

   GL_CALL(glGenVertexArrays(1, &cube_vao));
   GL_CALL(glGenBuffers(1, &cube_vbo));
   GL_CALL(glBindVertexArray(cube_vao));
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, cube_vbo));

   GL_CALL(glBindVertexArray(cube_vao));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW));
   GL_CALL(glEnableVertexAttribArray(0));
   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
}

void setup_quad()
{
   float vertices[] = {
           1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
           1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
           -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
           -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
   };
   GLuint indices[] = {
           0, 1, 3,
           1, 2, 3
   };

   GL_CALL(glGenVertexArrays(1, &quad_vao));
   GL_CALL(glGenBuffers(1, &quad_vbo));
   GL_CALL(glGenBuffers(1, &quad_ebo));
   GL_CALL(glBindVertexArray(quad_vao));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, quad_vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo));
   GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
   GL_CALL(glEnableVertexAttribArray(0));

   GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
   GL_CALL(glEnableVertexAttribArray(1));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void rc_build(render_chain_t* rc)
{
   for(size_t i = 0; i < rc->stages->count; i++)
   {
      render_stage_t* stage = rc->stages->collection[i];
      if(stage->render_mode == RM_GEOMETRY ||
         stage->render_mode == RM_FRAMEBUFFER ||
         stage->render_mode == RM_CUSTOM_FRAMEBUFFER)

         rs_build_tex(stage);

      if(i != 0)
         stage->prev_stage = rc->stages->collection[i - 1];
      else
         stage->prev_stage = NULL;
   }
}

render_chain_t* rc_default(win_info_t* info, camera_t* camera)
{
   if(!quad_vbo) setup_quad();
   if(!cube_vbo) setup_cube();

   mat4 proj_mat = cmat4();
   create_perspective(info, proj_mat, 65.0f, 0.1f, 200);

   shader_t* skybox_shader = s_get_shader(SH_SKYBOX);
   shader_t* default_shader = s_get_shader(SH_DEFAULT);
   shader_t* gaussian_shader = s_get_shader(SH_BLUR);
   shader_t* bypass_shader = s_get_shader(SH_BYPASS);

   render_stage_t* geometry = rs_create(RM_GEOMETRY, default_shader);
   geometry->tex_width = info->w;
   geometry->tex_height = info->h;
   geometry->bind_shader = bind_default;
   geometry->setup_obj_shader = setup_default;
   geometry->unbind_shader = unbind_default;
   geometry->attachments = TF_COLOR | TF_DEPTH;
   mat4_cpy(geometry->proj, proj_mat);
   mat4_identity(geometry->view);
   default_shader_data_t* geometry_data = (geometry->data = malloc(sizeof(default_shader_data_t)));
   geometry_data->buffmat = cmat4();
   geometry_data->camera = camera;

   render_stage_t* skybox = rs_create(RM_CUSTOM, skybox_shader);
   skybox->tex_width = info->w;
   skybox->tex_height = info->h;
   skybox->bind_shader = bind_skybox;
   skybox->custom_draw_func = draw_skybox;
   skybox->unbind_shader = unbind_skybox;
   mat4_cpy(skybox->proj, proj_mat);
   mat4_identity(skybox->view);
   skybox->vao = cube_vao;
   default_shader_data_t* skybox_data = (skybox->data = malloc(sizeof(default_shader_data_t)));
   skybox_data->buffmat = cmat4();
   skybox_data->camera = camera;

/*
   render_stage_t* blur = rs_create(false, bypass_shader);
   blur->tex_width = info->w;
   blur->tex_height = info->h;
   blur->bind_shader = bind_bypass;
   blur->unbind_shader = unbind_default;
   blur->vao = quad_vao;
   blur->ebo = quad_ebo;
   create_ortho(info, blur->proj, 0.1f, 200);
   mat4_identity(blur->view);
   mat4_identity(blur->proj);
*/

   render_stage_t* bypass = rs_create(RM_BYPASS, bypass_shader);
   bypass->tex_width = info->w;
   bypass->tex_height = info->h;
   bypass->bind_shader = bind_bypass;
   bypass->unbind_shader = unbind_bypass;
   bypass->vao = quad_vao;

   render_chain_t* rc = rc_create();
   list_push(rc->stages, geometry);
   list_push(rc->stages, skybox);
   //list_push(rc->stages, blur);
   list_push(rc->stages, bypass);

   mat4_free(proj_mat);

   rc_build(rc);
   return rc;
}


