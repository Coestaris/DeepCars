//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"
#include "../renderer.h"
#include "../shader_setup.h"

// FXAA ROUTINES
void bind_fxaa(render_stage_t* stage)
{
   render_stage_t* bypass = default_rc->stages->collection[STAGE_BYPASS];
   t_bind(bypass->color0_tex, UNIFORM_FXAA.tex);

   sh_set_int(UNIFORM_FXAA.show_edges, fxaa_edges);
   sh_set_int(UNIFORM_FXAA.on, fxaa_state);
}

void unbind_fxaa(render_stage_t* stage) { }

// GAMMA / BYPASS ROUTINES
void bind_bypass(render_stage_t* stage)
{
   render_stage_t* shading_stage = default_rc->stages->collection[STAGE_SHADING];
   if(render_state == 0)
   {
      t_bind(shading_stage->color0_tex, UNIFORM_GAMMA.tex);

      render_stage_t* gbuffer = default_rc->stages->collection[STAGE_G_BUFFER];
      t_bind(gbuffer->color0_tex, UNIFORM_GAMMA.depth_tex);

      sh_set_int(UNIFORM_GAMMA.postprocess, true);
   }
   else
   {
      t_bind(texture_to_draw, UNIFORM_GAMMA.tex);

      sh_set_int(UNIFORM_GAMMA.postprocess, false);
   }
}

void unbind_bypass(render_stage_t* stage) { }
