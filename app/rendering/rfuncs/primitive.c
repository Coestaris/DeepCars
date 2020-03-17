//
// Created by maxim on 3/17/20.
//
#include "rfuncs.h"
#include "../renderer.h"
#include "../vfx.h"

void bind_primitive(render_stage_t* stage)
{
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void unbind_primitive(render_stage_t* stage)
{
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
}

void draw_primitives(render_stage_t* stage)
{
   render_stage_t* rs = default_rc->stages->collection[STAGE_SHADING];

   sh_use(br_shader);

   for(size_t i = 0; i < blurred_regions->count; i++)
   {
      blurred_region_t* br = blurred_regions->collection[i];
      if(!br->visible)
         continue;

      GL_PCALL(glBindVertexArray(br->vao));
      if(br->gray_color)
         sh_set_vec3(UNIFORM_BR.gray_color, br->gray_color);
      sh_set_float(UNIFORM_BR.transparency, br->transparency);
      t_bind(br->back_tex, UNIFORM_BR.back_tex);
      t_bind(br->tex, UNIFORM_BR.tex);

      GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
   }
   GL_PCALL(glBindVertexArray(0));
   sh_use(NULL);

   gr_pq_flush();
}

void bind_sprite_renderer(sprite_renderer_t* this, mat4 transform, void* data)
{
   float trans = *((float*)data);
   sh_set_mat4(UNIFORM_SPRITE.model, transform);
   sh_set_float(UNIFORM_SPRITE.transparency, trans);
}

void bind_line_renderer(struct _primitive_renderer* this, float width, vec4 color, void* data)
{
   sh_set_vec3(UNIFORM_LINE_PRIMITIVE.color, color);
   sh_set_float(UNIFORM_LINE_PRIMITIVE.thickness, width);
}
