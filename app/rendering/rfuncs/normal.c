//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"

#include "../renderer.h"

// NORMAL ROUTINES
void bind_normal(render_stage_t* stage)
{
   sh_set_mat4(UNIFORM_NORMAL.view, render_view);
}

void unbind_normal(render_stage_t* stage) { }

void draw_normal(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_stage->fbo));
   list_t* objects = u_get_objects();

   //GL_PCALL(glEnable(GL_DEPTH_TEST));
   for(size_t i = 0; i < objects->count; i++)
   {
      object_t* object = objects->collection[i];
      if(object->draw_info->draw_normals)
      {
         gr_transform(object->position, object->scale, object->rotation);
         sh_set_mat4(UNIFORM_NORMAL.model, model_mat);

         GL_PCALL(glBindVertexArray(object->draw_info->normal_vao));

         GL_PCALL(glDrawArrays(GL_LINES, 0, object->draw_info->normal_buffer_len));

         GL_PCALL(glBindVertexArray(0));
      }
   }
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}