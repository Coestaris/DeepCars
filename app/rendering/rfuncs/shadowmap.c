//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"

// SHADOWMAP ROUTINES
void bind_shadowmap(render_stage_t* stage)
{
   scene_t* scene = scm_get_current();
   shadow_light_t* shadow_light = scene->shadow_light;

   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

   sh_set_mat4(UNIFORM_SHADOWMAP.light_space, shadow_light->light_space);

   GL_PCALL(glCullFace(GL_FRONT));
   //stage->skip = true;
}

void unbind_shadowmap(render_stage_t* stage)
{
   GL_PCALL(glCullFace(GL_BACK));
}

void setup_object_shadowmap(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_set_mat4(UNIFORM_SHADOWMAP.model, model_mat);
}