//
// Created by maxim on 3/17/20.
//

#include "rfuncs.h"
#include "../renderer.h"

// SSAO ROUTINES
void bind_ssao(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];

   t_bind(g_buffer_stage->color0_tex, UNIFORM_SSAO.pos_tex);
   t_bind(g_buffer_stage->color1_tex, UNIFORM_SSAO.norm_tex);
   t_bind(noise_texture, UNIFORM_SSAO.noise_tex);
}

void unbind_ssao(render_stage_t* stage) { }


// SSAO BLUR ROUTINES
void bind_ssao_blur(render_stage_t* stage)
{
   t_bind(stage->prev_stage->color0_tex, UNIFORM_SSAO_BLUR.tex);
}

void unbind_ssao_blur(render_stage_t* stage) { }
