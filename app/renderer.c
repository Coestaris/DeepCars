//
// Created by maxim on 12/30/19.
//

#ifdef __GNUC__
#pragma implementation "renderer.h"
#endif
#include "renderer.h"
#include "win_defaults.h"
#include "../lib/resources/rmanager.h"
#include "../lib/scene.h"
#include "../lib/scm.h"

render_chain_t* rc1;
render_chain_t* rc2;

typedef struct _geometry_shader_data {
   camera_t* camera;
   mat4 buffmat;

} geometry_shader_data_t;

void switch_stages()
{
   if(rc_get_current() == rc1) rc_set_current(rc2);
   else rc_set_current(rc1);
}

void bind_bypass(render_stage_t* stage)
{
   sh_nset_int(stage->shader, "tex", 0);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, stage->prev_stage->color_tex);
}

void unbind_bypass(render_stage_t* stage)
{
   t_bind(NULL, 0, GL_TEXTURE_2D);
}

void bind_depth_bypass(render_stage_t* stage)
{
   sh_nset_int(stage->shader, "depth_map", 0);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, stage->prev_stage->depth_tex);
   sh_nset_float(stage->shader, "near_plane", 1);
   sh_nset_float(stage->shader, "far_plane", 200);
}

void unbind_depth_bypass(render_stage_t* stage)
{
   t_bind(NULL, 0, GL_TEXTURE_2D);
}

void bind_geometry(render_stage_t* stage)
{
   GL_PCALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT));

   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   scene_t* scene = scm_get_current();

   mat4_cpy(scene->light->light_space, scene->light->light_proj);
   mat4_mulm(scene->light->light_space, scene->light->light_view);

   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_mat4(stage->shader, "view", data->buffmat);

   sh_nset_vec3(stage->shader, "viewPos", data->camera->position);
   sh_nset_vec3(stage->shader, "lightColor", scene->light->color);
   sh_nset_vec3(stage->shader, "lightPos", scene->light->position);
   sh_nset_mat4(stage->shader, "lightSpaceMatrix", scene->light->light_space);

   sh_nset_int(stage->shader, "shadowMap", 0);
   GL_PCALL(glActiveTexture(GL_TEXTURE0));
   GL_PCALL(glBindTexture(GL_TEXTURE_2D, stage->prev_stage->depth_tex));

   sh_nset_int(stage->shader, "diffuseTexture", 1);
   sh_nset_int(stage->shader, "specularTexture", 2);
   sh_nset_int(stage->shader, "ambientTexture", 3);
}

void unbind_geometry(render_stage_t* stage)
{
   t_bind(NULL, 0, GL_TEXTURE_2D);
   t_bind(NULL, 1, GL_TEXTURE_2D);
}

void setup_object(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   t_bind(object->draw_info->material->map_diffuse, 1, GL_TEXTURE_2D);
   t_bind(object->draw_info->material->map_specular, 2, GL_TEXTURE_2D);
   t_bind(object->draw_info->material->map_ambient, 3, GL_TEXTURE_2D);

   sh_nset_float(stage->shader, "shininess", object->draw_info->material->shininess);

   sh_nset_mat4(stage->shader, "model", model_mat);
}

void bind_skybox(render_stage_t* stage)
{
   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   scene_t* scene = scm_get_current();

   data->buffmat[3] = 0;
   data->buffmat[7] = 0;
   data->buffmat[11] = 0;
   data->buffmat[15] = 0;

   sh_nset_mat4(stage->shader, "view", data->buffmat);
   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_int(stage->shader, "skybox", 0);

   GL_PCALL(glActiveTexture(GL_TEXTURE0));
   t_bind(scene->skybox, 0, GL_TEXTURE_CUBE_MAP);
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
      GL_PCALL(glDepthFunc(GL_LEQUAL));
      GL_PCALL(glBindVertexArray(stage->vao));
      GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, 36));
      GL_PCALL(glBindVertexArray(0));
      GL_PCALL(glDepthFunc(GL_LESS));
   }
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void bind_depth(render_stage_t* stage)
{
   scene_t* scene = scm_get_current();

   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT));

   scene->light->light_camera->position = scene->light->position;
   c_to_mat(scene->light->light_view, scene->light->light_camera);
   mat4_cpy(scene->light->light_space, scene->light->light_proj);
   mat4_mulm(scene->light->light_space, scene->light->light_view);

   sh_nset_mat4(stage->shader, "lightSpaceMatrix", scene->light->light_space);
   GL_PCALL(glCullFace(GL_FRONT));
}

void unbind_depth(render_stage_t* stage)
{
   GL_PCALL(glCullFace(GL_BACK));
}

void setup_object_depth(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_nset_mat4(stage->shader, "model", model_mat);
}

render_chain_t* get_chain(win_info_t* info, camera_t* camera, mat4 proj)
{
   shader_t* skybox_shader = s_get_shader(SH_SKYBOX);
   shader_t* default_shader = s_get_shader(SH_DEFAULT);
   shader_t* bypass_shader = s_get_shader(SH_BYPASS);
   shader_t* gamma_shader = s_get_shader(SH_GAMMA);
   shader_t* depth_shader = s_get_shader(SH_DEPTH_SHADER);
   shader_t* depth_bypass_shader = s_get_shader(SH_DEPTH_BYPASS);

   render_stage_t* depth = rs_create(RM_GEOMETRY, depth_shader);
   depth->tex_width = 2048;
   depth->tex_height = 2048;
   depth->bind_shader = bind_depth;
   depth->setup_obj_shader = setup_object_depth;
   depth->unbind_shader = unbind_depth;
   depth->attachments = TF_DEPTH;

   render_stage_t* depth_bypass = rs_create(RM_BYPASS, depth_bypass_shader);
   depth_bypass->tex_width = info->w;
   depth_bypass->tex_height = info->h;
   depth_bypass->bind_shader = bind_depth_bypass;
   depth_bypass->unbind_shader = unbind_depth_bypass;
   depth_bypass->vao = rc_get_quad_vao();

   render_stage_t* geometry = rs_create(RM_GEOMETRY, default_shader);
   geometry->tex_width = info->w;
   geometry->tex_height = info->h;
   geometry->bind_shader = bind_geometry;
   geometry->setup_obj_shader = setup_object;
   geometry->unbind_shader = unbind_geometry;
   geometry->attachments = TF_COLOR | TF_DEPTH;
   mat4_cpy(geometry->proj, proj);
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
   mat4_cpy(skybox->proj, proj);
   mat4_identity(skybox->view);
   skybox->vao = rc_get_cube_vao();
   geometry_shader_data_t* skybox_data = (skybox->data = malloc(sizeof(geometry_shader_data_t)));
   skybox_data->buffmat = cmat4();
   skybox_data->camera = camera;

   render_stage_t* bypass = rs_create(RM_BYPASS, gamma_shader);
   bypass->tex_width = info->w;
   bypass->tex_height = info->h;
   bypass->bind_shader = bind_bypass;
   bypass->unbind_shader = unbind_bypass;
   bypass->vao = rc_get_quad_vao();

   rc1 = rc_create();
   list_push(rc1->stages, depth);
   list_push(rc1->stages, geometry);
   list_push(rc1->stages, skybox);
   list_push(rc1->stages, bypass);

   rc2 = rc_create();
   list_push(rc2->stages, depth);
   list_push(rc2->stages, depth_bypass);

   rc_build(rc1);
   rc_build(rc2);

   rc_link(rc1);

   GL_PCALL(glEnable(GL_DEPTH_TEST));

   return rc1;
}

void free_stages(void)
{
   rs_free(rc1->stages->collection[0]); // depth
   rs_free(rc1->stages->collection[1]); // geometry
   rs_free(rc1->stages->collection[2]); // skybox
   rs_free(rc1->stages->collection[3]); // bypass

   rs_free(rc2->stages->collection[1]); // depth_bypass

   rc_free(rc1, false);
   rc_free(rc2, false);
}
