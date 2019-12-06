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
   glBindTexture(GL_TEXTURE_2D, stage->prev_stage->tex);
}

void unbind_bypass(render_stage_t* stage)
{
   t_bind(NULL, 0);
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

}
void setup_default(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_nset_vec3v(stage->shader, "objectColor",
           object->draw_info->object_color[0],
           object->draw_info->object_color[1],
           object->draw_info->object_color[2]);
   sh_nset_mat4(stage->shader, "model", model_mat);
}
GLuint quad_vao;
GLuint quad_vbo;
GLuint quad_ebo;

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

render_chain_t* rc_default(win_info_t* info, camera_t* camera)
{
   setup_quad();

   shader_t* default_shader = s_get_shader(SH_DEFAULT);
   //shader_t* gaussian_shader = s_get_shader(SH_BLUR);
   shader_t* bypass_shader = s_get_shader(SH_BYPASS);

   render_stage_t* geometry = rs_create(true, default_shader);
   geometry->tex_width = info->w;
   geometry->tex_height = info->h;
   geometry->bind_shader = bind_default;
   geometry->setup_obj_shader = setup_default;
   geometry->unbind_shader = unbind_default;
   create_perspective(info, geometry->proj, 65.0f, 0.1f, 200);
   mat4_identity(geometry->view);
   default_shader_data_t* data = (geometry->data = malloc(sizeof(default_shader_data_t)));
   data->buffmat = cmat4();
   data->camera = camera;

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

   render_stage_t* bypass = rs_create(false, bypass_shader);
   bypass->tex_width = info->w;
   bypass->tex_height = info->h;
   bypass->bind_shader = bind_bypass;
   bypass->unbind_shader = unbind_bypass;
   bypass->vao = quad_vao;
   bypass->ebo = quad_ebo;
   mat4_identity(bypass->view);
   mat4_identity(bypass->proj);
   bypass->final = true;

   render_chain_t* rc = rc_create();
   list_push(rc->stages, geometry);
   //list_push(rc->stages, blur);
   list_push(rc->stages, bypass);

   rs_build_tex(geometry);
   //rs_build_tex(blur);
   //rs_build_tex(bypass);
   //blur->prev_stage = geometry;
   bypass->prev_stage = geometry;
   return rc;
}


