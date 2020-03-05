//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_stage.h"
#endif
#include "render_stage.h"
#include "../../resources/rmanager.h"

#define RS_LOG(format, ...) DC_LOG("render_stage.c", format, __VA_ARGS__)
#define RS_ERROR(format, ...) DC_ERROR("render_stage.c", format, __VA_ARGS__)

static void rs_set_color_options(attachment_options_t* ao)
{
   ao->tex_width = 1024;
   ao->tex_height = 1024;
   ao->tex_min_filter = GL_NEAREST;
   ao->tex_mag_filter = GL_NEAREST;
   ao->tex_wrapping_s = GL_REPEAT;
   ao->tex_wrapping_t = GL_REPEAT;
   ao->tex_int_format = GL_RGBA;
   ao->tex_format = GL_RGBA;
   ao->tex_border_color[0] = 0;
   ao->tex_border_color[1] = 0;
   ao->tex_border_color[2] = 0;
   ao->tex_border_color[3] = 0;
   ao->tex_target = GL_TEXTURE_2D;
}

static void rs_set_depth_options(attachment_options_t* ao)
{
   ao->tex_width = 1024;
   ao->tex_height = 1024;
   ao->tex_min_filter = GL_NEAREST;
   ao->tex_mag_filter = GL_NEAREST;
   ao->tex_wrapping_s = GL_CLAMP_TO_BORDER;
   ao->tex_wrapping_t = GL_CLAMP_TO_BORDER;
   ao->tex_int_format = GL_DEPTH_COMPONENT;
   ao->tex_format = GL_DEPTH_COMPONENT;
   ao->tex_border_color[0] = 0;
   ao->tex_border_color[1] = 0;
   ao->tex_border_color[2] = 0;
   ao->tex_border_color[3] = 0;
   ao->tex_target = GL_TEXTURE_2D;
}

render_stage_t* rs_create(const char* name, render_mode_t render_mode, shader_t* shader)
{
   render_stage_t* rs = DEEPCARS_MALLOC(sizeof(render_stage_t));
   rs->shader = shader;
   rs->render_mode = render_mode;
   rs->name = name;

   rs->bind_func      = NULL;
   rs->setup_obj_func = NULL;
   rs->unbind_func    = NULL;
   rs->data           = NULL;
   rs->skip           = false;

   rs->attachments = 0;
   rs_set_color_options(&rs->color0_format);
   rs_set_color_options(&rs->color1_format);
   rs_set_color_options(&rs->color2_format);
   rs_set_color_options(&rs->color3_format);
   rs_set_color_options(&rs->color4_format);
   rs_set_color_options(&rs->color5_format);
   rs_set_depth_options(&rs->depth_format);
   rs_set_color_options(&rs->stencil_format);

   rs->width = 0;
   rs->height = 0;

   rs->fbo = 0;
   rs->vao = 0;

   rs->color0_tex    = NULL;
   rs->color1_tex    = NULL;
   rs->color2_tex    = NULL;
   rs->color3_tex    = NULL;
   rs->color4_tex    = NULL;
   rs->color5_tex    = NULL;
   rs->depth_tex     = NULL;
   rs->stencil_tex   = NULL;

   rs->proj = cmat4();
   rs->view = cmat4();

#ifdef MEASURE_RENDER_TIME
   rs->render_time = 0;
#endif

   return rs;
}

void rs_free(render_stage_t* rs)
{
   if(rs->fbo)
   GL_CALL(glDeleteFramebuffers(1, &rs->fbo));

   mat4_free(rs->proj);
   mat4_free(rs->view);

   if(rs->data) DEEPCARS_FREE(rs->data);
   DEEPCARS_FREE(rs);
}

static texture_t* rs_setup_tex(GLenum attachment, attachment_options_t options, GLuint fbo, const char* target)
{
   GLuint id;
   GL_CALL(glGenTextures(1, &id));
   GL_CALL(glBindTexture(options.tex_target, id));
   if(options.tex_target == GL_TEXTURE_2D_MULTISAMPLE)
   {
      GL_CALL(glTexImage2DMultisample(options.tex_target, 4, options.tex_int_format,
                                      options.tex_width, options.tex_height, GL_TRUE));
   }
   else
   {
      GL_CALL(glTexImage2D(options.tex_target, 0, options.tex_int_format,
                           options.tex_width, options.tex_height, 0, options.tex_format, GL_FLOAT, NULL));

      GL_CALL(glTexParameteri(options.tex_target, GL_TEXTURE_MIN_FILTER, options.tex_min_filter));
      GL_CALL(glTexParameteri(options.tex_target, GL_TEXTURE_MAG_FILTER, options.tex_mag_filter));
      GL_CALL(glTexParameteri(options.tex_target, GL_TEXTURE_WRAP_S, options.tex_wrapping_s));
      GL_CALL(glTexParameteri(options.tex_target, GL_TEXTURE_WRAP_T, options.tex_wrapping_t));
      GL_CALL(glTexParameterfv(options.tex_target, GL_TEXTURE_BORDER_COLOR, options.tex_border_color));
   }

   GL_CALL(glBindTexture(options.tex_target, 0));

   GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
   GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, options.tex_target, id, 0));
   GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

   char* name = DEEPCARS_MALLOC(30);
   snprintf(name, 30, "__generated_fb%i_%s", fbo, target);
   texture_t* t = t_create(name);
   t->type = GL_TEXTURE_2D;
   t->height = options.tex_height;
   t->width = options.tex_width;
   t->texID = id;
   rm_push(TEXTURE, t, -1);

   return t;
}

static void rs_check_sizes(render_stage_t* rs, attachment_options_t options)
{
   if(!rs->width)
   {
      rs->width = options.tex_width;
      rs->height = options.tex_height;
   }
   else
   {
      if(rs->width  != (GLuint)options.tex_width ||
         rs->height != (GLuint)options.tex_height)
      {
         RS_ERROR("Sizes of attachments should be equal, I guess....",0);
      }
   }
}

void rs_build_tex(render_stage_t* rs)
{
   if(!rs->fbo)
   GL_CALL(glGenFramebuffers(1, &rs->fbo));

   size_t count = 0;
   GLenum attachments[10];
   if(rs->attachments & TF_COLOR0 && !rs->color0_tex)
   {
      rs->color0_tex = rs_setup_tex(GL_COLOR_ATTACHMENT0, rs->color0_format, rs->fbo, "color0");
      rs_check_sizes(rs, rs->color0_format);
      attachments[count++] = GL_COLOR_ATTACHMENT0;
   }

   if(rs->attachments & TF_COLOR1 && !rs->color1_tex)
   {
      rs->color1_tex =rs_setup_tex(GL_COLOR_ATTACHMENT1, rs->color1_format, rs->fbo, "color1");
      rs_check_sizes(rs, rs->color1_format);
      attachments[count++] = GL_COLOR_ATTACHMENT1;
   }

   if(rs->attachments & TF_COLOR2 && !rs->color2_tex)
   {
      rs->color2_tex =rs_setup_tex(GL_COLOR_ATTACHMENT2, rs->color2_format, rs->fbo, "color2");
      rs_check_sizes(rs, rs->color2_format);
      attachments[count++] = GL_COLOR_ATTACHMENT2;
   }

   if(rs->attachments & TF_COLOR3 && !rs->color3_tex)
   {
      rs->color3_tex =rs_setup_tex(GL_COLOR_ATTACHMENT3, rs->color3_format, rs->fbo, "color3");
      rs_check_sizes(rs, rs->color3_format);
      attachments[count++] = GL_COLOR_ATTACHMENT3;
   }

   if(rs->attachments & TF_COLOR4 && !rs->color4_tex)
   {
      rs->color4_tex =rs_setup_tex(GL_COLOR_ATTACHMENT4, rs->color4_format, rs->fbo, "color4");
      rs_check_sizes(rs, rs->color4_format);
      attachments[count++] = GL_COLOR_ATTACHMENT4;
   }

   if(rs->attachments & TF_COLOR5 && !rs->color5_tex)
   {
      rs->color5_tex =rs_setup_tex(GL_COLOR_ATTACHMENT5, rs->color5_format, rs->fbo, "color5");
      rs_check_sizes(rs, rs->color5_format);
      attachments[count++] = GL_COLOR_ATTACHMENT5;
   }

   if(rs->attachments & TF_DEPTH && !rs->depth_tex)
   {
      rs->depth_tex = rs_setup_tex(GL_DEPTH_ATTACHMENT, rs->depth_format, rs->fbo, "depth");
      rs_check_sizes(rs, rs->depth_format);
      attachments[count++] = GL_DEPTH_ATTACHMENT;
   }

   if(rs->attachments & TF_STENCIL && !rs->stencil_tex)
   {
      rs->stencil_tex = rs_setup_tex(GL_DEPTH_ATTACHMENT, rs->stencil_format, rs->fbo, "stencil");
      rs_check_sizes(rs, rs->stencil_format);
      attachments[count++] = GL_DEPTH_ATTACHMENT;
   }

   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, rs->fbo));
   if(count > 2)
   GL_PCALL(glDrawBuffers(count - 1, attachments));

   GLenum error;
   if ((error = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
      RS_ERROR("glCheckFramebufferStatus returned \"%i\"", error);

   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
