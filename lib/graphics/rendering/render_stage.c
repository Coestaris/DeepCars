//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_stage.h"
#endif
#include "render_stage.h"

render_stage_t* rs_create(render_mode_t render_mode, shader_t* shader)
{
   render_stage_t* rs = malloc(sizeof(render_stage_t));
   rs->shader = shader;
   rs->render_mode = render_mode;

   rs->bind_shader = NULL;
   rs->setup_obj_shader = NULL;
   rs->unbind_shader = NULL;
   rs->data = NULL;

   rs->tex_width = 1024;
   rs->tex_height = 1024;
   rs->attachments = 0;
   rs->tex_min_filter = GL_NEAREST;
   rs->tex_mag_filter = GL_NEAREST;
   rs->tex_wrapping = GL_REPEAT;
   rs->fbo = 0;
   rs->vao = 0;

   rs->color_tex = 0;
   rs->depth_tex = 0;
   rs->stencil_tex = 0;

   rs->proj = cmat4();
   rs->view = cmat4();

   return rs;
}

void rs_free(render_stage_t* rs)
{
   if(rs->fbo) GL_CALL(glDeleteFramebuffers(1, &rs->fbo));
   if(rs->color_tex) GL_CALL(glDeleteTextures(1, &rs->color_tex));
   if(rs->depth_tex) GL_CALL(glDeleteTextures(1, &rs->color_tex));
   if(rs->stencil_tex) GL_CALL(glDeleteTextures(1, &rs->color_tex));

   mat4_free(rs->proj);
   mat4_free(rs->view);

   if(rs->data) free(rs->data);
   free(rs);
}

void rs_build_tex(render_stage_t* rs)
{
   if(!rs->fbo)
      GL_CALL(glGenFramebuffers(1, &rs->fbo));

   if(rs->attachments & TF_COLOR && !rs->color_tex)
   {
      GL_CALL(glGenTextures(1, &rs->color_tex));
      GL_CALL(glBindTexture(GL_TEXTURE_2D, rs->color_tex));
      GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                           rs->tex_width, rs->tex_height, 0, GL_RGBA, GL_FLOAT, NULL));

      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rs->tex_min_filter));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rs->tex_mag_filter));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, rs->tex_wrapping));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, rs->tex_wrapping));
      GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

      GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rs->fbo));
      GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rs->color_tex, 0));
      GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }

   if(rs->attachments & TF_DEPTH && !rs->depth_tex)
   {
      GL_CALL(glGenTextures(1, &rs->depth_tex));
      GL_CALL(glBindTexture(GL_TEXTURE_2D, rs->depth_tex));
      GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                   rs->tex_width, rs->tex_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rs->tex_min_filter));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rs->tex_mag_filter));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, rs->tex_wrapping));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, rs->tex_wrapping));

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

      GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rs->fbo));
      GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rs->depth_tex, 0));
      GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }

   if(rs->attachments & TF_STENCIL && !rs->stencil_tex)
   {
      GL_CALL(glGenTextures(1, &rs->stencil_tex));
      GL_CALL(glBindTexture(GL_TEXTURE_2D, rs->stencil_tex));
      GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_COMPONENTS,
                           rs->tex_width, rs->tex_height, 0, GL_STENCIL_COMPONENTS, GL_FLOAT, NULL));

      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rs->tex_min_filter));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rs->tex_mag_filter));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, rs->tex_wrapping));
      GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, rs->tex_wrapping));
      GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

      GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rs->fbo));
      GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rs->stencil_tex, 0));
      GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }
}
