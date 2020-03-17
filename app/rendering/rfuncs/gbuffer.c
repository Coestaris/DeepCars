//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"
#include "../renderer.h"

mat4 render_view = NULL;

// G BUFFER ROUTINES
void bind_g_buffer(render_stage_t* stage)
{
   GL_PCALL(glClearColor(0, 0, 0, 0));
   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

   if(wireframe)
      GL_PCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(render_view, data->camera);
   sh_set_mat4(UNIFORM_GBUFF.view, render_view);
}

void unbind_g_buffer(render_stage_t* stage)
{
   if(wireframe)
      GL_PCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

void setup_object_g_buffer(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   t_bind(object->draw_info->material->map_diffuse, UNIFORM_GBUFF.diffuse_tex);
   t_bind(object->draw_info->material->map_specular, UNIFORM_GBUFF.spec_tex);
   sh_set_mat4(UNIFORM_GBUFF.model, model_mat);
}

void bind_g_buffer_inst(render_stage_t* stage)
{
   sh_set_mat4(UNIFORM_GBUFF_INST.view, render_view);

   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_stage->fbo));
}

void unbind_g_buffer_inst(render_stage_t* stage)
{
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void setup_g_buffer_inst(render_stage_t* stage, instance_collection_t* ic)
{
   t_bind(ic->material->map_diffuse, UNIFORM_GBUFF_INST.diffuse_tex);
   t_bind(ic->material->map_specular, UNIFORM_GBUFF_INST.spec_tex);
}