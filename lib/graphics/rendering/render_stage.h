//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_STAGE_H
#define DEEPCARS_RENDER_STAGE_H

#include "../../object.h"
#include "../../shaders/shader.h"

typedef enum _render_mode {
   RM_CUSTOM,
   RM_CUSTOM_FRAMEBUFFER,
   RM_GEOMETRY,
   RM_FRAMEBUFFER,
   RM_BYPASS

} render_mode_t;

typedef enum tex_format {
  TF_COLOR = 1,
  TF_STENCIL = 2,
  TF_DEPTH = 4,
} tex_format_t;

typedef struct _render_stage {

   mat4 proj;
   mat4 view;

   render_mode_t render_mode;
   shader_t* shader;

   void (*setup_obj_shader)(struct _render_stage* this, object_t* render_obj, mat4 model_mat);
   void (*bind_shader)(struct _render_stage* this);
   void (*unbind_shader)(struct _render_stage* this);
   void (*custom_draw_func)(struct _render_stage* this);

   void* data;

   GLint tex_width;
   GLint tex_height;
   GLenum tex_min_filter;
   GLenum tex_mag_filter;
   GLenum tex_wrapping;

   tex_format_t attachments;

   GLuint fbo;
   GLuint color_tex;
   GLuint depth_tex;
   GLuint stencil_tex;

   GLuint vao;

   struct _render_stage* prev_stage;

} render_stage_t;

render_stage_t* rs_create(render_mode_t render_mode, shader_t* shader);

void rs_free(render_stage_t* rs);

void rs_build_tex(render_stage_t* rs);

#endif //DEEPCARS_RENDER_STAGE_H
