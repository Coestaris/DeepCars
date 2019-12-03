//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_STAGE_H
#define DEEPCARS_RENDER_STAGE_H

#include "../../object.h"
#include "../../shaders/shader.h"

typedef struct _render_stage {

   mat4 proj;
   mat4 view;

   bool render_geometry;
   shader_t* shader;

   void (*setup_obj_shader)(struct _render_stage this, object_t* render_obj);
   void (*bind_shader)(struct _render_stage this);
   void (*unbind_shader)(struct _render_stage this);

   void* data;
   bool final;

   GLint tex_width;
   GLint tex_height;
   GLenum tex_format;
   GLenum tex_min_filter;
   GLenum tex_mag_filter;
   GLenum tex_wrapping;

   GLuint fbo;
   GLuint tex;

   struct _render_stage* prev_stage;

} render_stage_t;

render_stage_t* rs_create(bool render_geometry, shader_t* shader);
void rs_free(render_stage_t* rs);
void rs_build_tex(render_stage_t* rs);

#endif //DEEPCARS_RENDER_STAGE_H
