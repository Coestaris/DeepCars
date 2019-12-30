//
// Created by maxim on 12/30/19.
//

#ifdef __GNUC__
#pragma implementation "renderer.h"
#endif
#include "renderer.h"
#include "../lib/resources/txm.h"

typedef struct _geometry_shader_data {
   camera_t* camera;
   mat4 buffmat;

} geometry_shader_data_t;


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

void bind_geometry(render_stage_t* stage)
{
   GL_PCALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT));

   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_mat4(stage->shader, "view", data->buffmat);
}

void unbind_geometry(render_stage_t* stage)
{

}

void setup_object(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_nset_vec3v(stage->shader, "objectColor",
                 object->draw_info->object_color[0],
                 object->draw_info->object_color[1],
                 object->draw_info->object_color[2]);
   sh_nset_mat4(stage->shader, "model", model_mat);
}

void bind_skybox(render_stage_t* stage)
{
   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
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
   {
      GL_PCALL(glEnable(GL_DEPTH_TEST));
      GL_PCALL(glDepthFunc(GL_LEQUAL));
      GL_PCALL(glBindVertexArray(stage->vao));
      GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, 36));
      GL_PCALL(glBindVertexArray(0));
      GL_PCALL(glDepthFunc(GL_LESS));
      GL_PCALL(glDisable(GL_DEPTH_TEST));
   }
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

render_chain_t* get_chain(win_info_t* info, camera_t* camera)
{
   mat4 proj_mat = cmat4();
   rc_create_perspective(info, proj_mat, 65.0f, 0.1f, 200);

   shader_t* skybox_shader = s_get_shader(SH_SKYBOX);
   shader_t* default_shader = s_get_shader(SH_DEFAULT);
   shader_t* gaussian_shader = s_get_shader(SH_BLUR);
   shader_t* bypass_shader = s_get_shader(SH_BYPASS);

   render_stage_t* geometry = rs_create(RM_GEOMETRY, default_shader);
   geometry->tex_width = info->w;
   geometry->tex_height = info->h;
   geometry->bind_shader = bind_geometry;
   geometry->setup_obj_shader = setup_object;
   geometry->unbind_shader = unbind_geometry;
   geometry->attachments = TF_COLOR | TF_DEPTH;
   mat4_cpy(geometry->proj, proj_mat);
   mat4_identity(geometry->view);
   geometry_shader_data_t* geometry_data = (geometry->data = malloc(sizeof(geometry_shader_data_t)));
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
   skybox->vao = rc_get_cube_vao();
   geometry_shader_data_t* skybox_data = (skybox->data = malloc(sizeof(geometry_shader_data_t)));
   skybox_data->buffmat = cmat4();
   skybox_data->camera = camera;

/*
   render_stage_t* blur = rs_create(false, bypass_shader);
   blur->tex_width = info->w;
   blur->tex_height = info->h;
   blur->bind_shader = bind_bypass;
   blur->unbind_shader = unbind_geometry;
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
   bypass->vao = rc_get_quad_vao();

   render_chain_t* rc = rc_create();
   list_push(rc->stages, geometry);
   list_push(rc->stages, skybox);
   //list_push(rc->stages, blur);
   list_push(rc->stages, bypass);

   mat4_free(proj_mat);

   rc_build(rc);
   return rc;
}
