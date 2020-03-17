//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"
#include "../renderer.h"

// SHADING ROUTINES
void bind_shading(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   render_stage_t* shadowmap_stage = default_rc->stages->collection[STAGE_SHADOWMAP];

   scene_t* scene = scm_get_current();
   geometry_shader_data_t* data = stage->data;

   sh_set_vec3(UNIFORM_SHADING.view_pos, data->camera->position);
   sh_set_mat4(UNIFORM_SHADING.view, render_view);

   //t_bind(g_buffer_stage->color0_tex, uniform);
   t_bind(g_buffer_stage->color1_tex, UNIFORM_SHADING.norm_tex);
   t_bind(g_buffer_stage->color2_tex, UNIFORM_SHADING.albedoSpec_tex);
   t_bind(ssao_texture, UNIFORM_SHADING.ssao_tex);
   t_bind(g_buffer_stage->color3_tex, UNIFORM_SHADING.pos_tex);
   t_bind(shadowmap_stage->depth_tex, UNIFORM_SHADING.shadow_light_shadowmap_tex);
}

void unbind_shading(render_stage_t* stage) { }