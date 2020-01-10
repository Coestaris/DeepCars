//
// Created by maxim on 11/30/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#ifndef DEEPCARS_RENDER_STAGE_H
#define DEEPCARS_RENDER_STAGE_H

#include "../../object.h"
#include "../../resources/shader.h"

typedef enum _render_mode {
   RM_CUSTOM,
   RM_CUSTOM_FRAMEBUFFER,
   RM_GEOMETRY,
   RM_FRAMEBUFFER,
   RM_BYPASS

} render_mode_t;

typedef enum tex_format {
  TF_STENCIL   = 1,
  TF_DEPTH     = 2,
  TF_COLOR0    = 4,
  TF_COLOR1    = 8,
  TF_COLOR2    = 16,
  TF_COLOR3    = 32,
  TF_COLOR4    = 64,
  TF_COLOR5    = 128,

} tex_format_t;

typedef struct _attachment_options {

   GLenum tex_format;
   GLenum tex_int_format;

   GLint tex_width;
   GLint tex_height;
   GLenum tex_min_filter;
   GLenum tex_mag_filter;
   GLenum tex_wrapping_s;
   GLenum tex_wrapping_t;

   float tex_border_color[4];

} attachment_options_t;

typedef struct _render_stage {

   mat4 proj;
   mat4 view;

   render_mode_t render_mode;
   shader_t* shader;

   bool skip;

   void (*setup_obj_func)(struct _render_stage* this, object_t* render_obj, mat4 model_mat);
   void (*bind_func)(struct _render_stage* this);
   void (*unbind_func)(struct _render_stage* this);
   void (*custom_draw_func)(struct _render_stage* this);

   void* data;

   tex_format_t attachments;

   GLuint width;
   GLuint height;

   GLuint fbo;

   texture_t* color0_tex;
   attachment_options_t color0_format;

   texture_t* color1_tex;
   attachment_options_t color1_format;

   texture_t* color2_tex;
   attachment_options_t color2_format;

   texture_t* color3_tex;
   attachment_options_t color3_format;

   texture_t* color4_tex;
   attachment_options_t color4_format;

   texture_t* color5_tex;
   attachment_options_t color5_format;

   texture_t* depth_tex;
   attachment_options_t depth_format;

   texture_t* stencil_tex;
   attachment_options_t stencil_format;

   GLuint vao;

   struct _render_stage* prev_stage;

} render_stage_t;

render_stage_t* rs_create(render_mode_t render_mode, shader_t* shader);

void rs_free(render_stage_t* rs);

void rs_build_tex(render_stage_t* rs);

#endif //DEEPCARS_RENDER_STAGE_H

#pragma clang diagnostic pop