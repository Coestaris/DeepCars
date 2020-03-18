//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"
#include "../renderer.h"

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
}

void setup_object_shadowmap(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_set_mat4(UNIFORM_SHADOWMAP.model, model_mat);
}

void bind_shadowmap_instanced(render_stage_t* stage)
{
   scene_t* scene = scm_get_current();
   shadow_light_t* shadow_light = scene->shadow_light;

   sh_set_mat4(UNIFORM_SHADOWMAP_INST.light_space, shadow_light->light_space);

   render_stage_t* shadowmap_stage = default_rc->stages->collection[STAGE_SHADOWMAP];
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, shadowmap_stage->fbo));
}

void unbind_shadowmap_instanced(render_stage_t* stage)
{
   GL_PCALL(glCullFace(GL_BACK));
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void setup_shadowmap_instanced(render_stage_t* stage, instance_collection_t* ic)
{

}