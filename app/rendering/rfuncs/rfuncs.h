//
// Created by maxim on 3/17/20.
//

#ifndef DEEPCARS_RFUNCS_H
#define DEEPCARS_RFUNCS_H

#include "../../../lib/graphics/rendering/render_stage.h"
#include "../../../lib/updater.h"
#include "../../../lib/scene.h"
#include "../../../lib/scm.h"

#include "../shader_setup.h"

extern mat4       render_view;

typedef struct _geometry_shader_data
{
   camera_t* camera;
   mat4 buffmat;

} geometry_shader_data_t;

// G_BUFFER
void bind_g_buffer(render_stage_t* stage);
void unbind_g_buffer(render_stage_t* stage);
void setup_object_g_buffer(render_stage_t* stage, object_t* object, mat4 model_mat);

void bind_normal(render_stage_t* stage);
void unbind_normal(render_stage_t* stage);
void draw_normal(render_stage_t* stage);

void bind_ssao(render_stage_t* stage);
void unbind_ssao(render_stage_t* stage);
void bind_ssao_blur(render_stage_t* stage);
void unbind_ssao_blur(render_stage_t* stage);

void bind_skybox(render_stage_t* stage);
void unbind_skybox(render_stage_t* stage);
void draw_skybox(render_stage_t* stage);

void bind_shadowmap(render_stage_t* stage);
void unbind_shadowmap(render_stage_t* stage);
void setup_object_shadowmap(render_stage_t* stage, object_t* object, mat4 model_mat);

void bind_shading(render_stage_t* stage);
void unbind_shading(render_stage_t* stage);

void bind_fxaa(render_stage_t* stage);
void unbind_fxaa(render_stage_t* stage);
void bind_bypass(render_stage_t* stage);
void unbind_bypass(render_stage_t* stage);

void bind_primitive(render_stage_t* stage);
void unbind_primitive(render_stage_t* stage);
void draw_primitives(render_stage_t* stage);

void bind_sprite_renderer(sprite_renderer_t* this, mat4 transform, void* data);
void bind_line_renderer(struct _primitive_renderer* this, float width, vec4 color, void* data);

#endif //DEEPCARS_RFUNCS_H
