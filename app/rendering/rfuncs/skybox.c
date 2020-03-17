//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"
#include "../renderer.h"

// SKYBOX ROUTINES
void bind_skybox(render_stage_t* stage)
{
   scene_t* scene = scm_get_current();
   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   mat4_cpy(data->buffmat, render_view);

   data->buffmat[3] = 0;
   data->buffmat[7] = 0;
   data->buffmat[11] = 0;
   data->buffmat[15] = 0;

   sh_set_mat4(UNIFORM_SKYBOX.view, data->buffmat);
   t_bind(scene->skybox, UNIFORM_SKYBOX.skybox_tex);
}

void unbind_skybox(render_stage_t* stage)
{
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void draw_skybox(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_stage->fbo));
   {
      GL_PCALL(glDepthFunc(GL_LEQUAL));
      GL_PCALL(glBindVertexArray(stage->vao));
      GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, 36));
      GL_PCALL(glBindVertexArray(0));
      GL_PCALL(glDepthFunc(GL_LESS));
   }
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}