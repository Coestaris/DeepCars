//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_stage.h"
#endif
#include "render_stage.h"

render_stage_t* rs_create(bool render_geometry, shader_t* shader)
{
   render_stage_t* rs = malloc(sizeof(render_stage_t));
   rs->shader = shader;
   rs->render_geometry = render_geometry;

   rs->bind_shader = NULL;
   rs->setup_obj_shader = NULL;
   rs->unbind_shader = NULL;
   rs->data = NULL;

   rs->tex_width = 1024;
   rs->tex_height = 1024;
   rs->tex_format = GL_RGBA;
   rs->tex_min_filter = GL_NEAREST;
   rs->tex_mag_filter = GL_NEAREST;
   rs->tex_wrapping = GL_REPEAT;
   rs->fbo = 0;
   rs->vao = 0;
   rs->tex = 0;

   rs->proj = cmat4();
   rs->view = cmat4();

   return rs;
}

void rs_free(render_stage_t* rs)
{
   if(rs->fbo) GL_CALL(glDeleteFramebuffers(1, &rs->fbo));
   if(rs->tex) GL_CALL(glDeleteTextures(1, &rs->tex));

   mat4_free(rs->proj);
   mat4_free(rs->view);

   if(rs->data) free(rs->data);
   free(rs);
}

void rs_build_tex(render_stage_t* rs)
{
   GL_CALL(glGenTextures(1, &rs->tex));
   GL_CALL(glBindTexture(GL_TEXTURE_2D, rs->tex));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rs->tex_min_filter));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rs->tex_mag_filter));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, rs->tex_wrapping));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, rs->tex_wrapping));
   GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, rs->tex_format,
                        rs->tex_width, rs->tex_height, 0, rs->tex_format, GL_FLOAT, NULL));

   GL_CALL(glGenFramebuffers(1, &rs->fbo));
   GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rs->fbo));

   GLenum attachment;
   switch(rs->tex_format)
   {
      default:
      case GL_RED:
      case GL_RG:
      case GL_RGB:
      case GL_BGR:
      case GL_RGBA:
      case GL_BGRA:
         attachment = GL_COLOR_ATTACHMENT0;
         break;
      case GL_STENCIL_INDEX:
         attachment = GL_STENCIL_ATTACHMENT;
         break;
      case GL_DEPTH_COMPONENT:
         attachment = GL_DEPTH_ATTACHMENT;
         break;
      case GL_DEPTH_STENCIL:
         attachment = GL_DEPTH_STENCIL_ATTACHMENT;
         break;
   }

   GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, rs->tex, 0));
   //GL_CALL(glDrawBuffer(GL_NONE));
   //GL_CALL(glReadBuffer(GL_NONE));
   GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}