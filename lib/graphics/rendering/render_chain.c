//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_chain.h"
#endif
#include "render_chain.h"

#include "../win.h"

// Global current rendering chain
static render_chain_t* current_chain;

//
// rc_free()
//
void rc_free(render_chain_t* rc, bool free_stages)
{
   ASSERT(rc);

   if(free_stages)
   {
      for(size_t i = 0; i < rc->stages->count; i++)
         rs_free(rc->stages->collection[i]);
   }
   if(rc->data) DEEPCARS_FREE(rc->data);
   list_free(rc->stages);
   DEEPCARS_FREE(rc);
}

//
// rc_create()
//
render_chain_t* rc_create()
{
   render_chain_t* this = DEEPCARS_MALLOC(sizeof(render_chain_t));
   this->data = NULL;
   this->stages = list_create();
   return this;
}

//
// rc_set_current()
//
void rc_set_current(render_chain_t* rc)
{
   ASSERT(rc);

   rc_link(rc);
   current_chain = rc;
}

//
// rc_get_current()
//
render_chain_t* rc_get_current(void)
{
   return current_chain;
}

//
// rc_link()
//
void rc_link(render_chain_t* rc)
{
   ASSERT(rc);

   for(size_t i = 0; i < rc->stages->count; i++)
   {
      render_stage_t* stage = rc->stages->collection[i];
      if(i != 0)
         stage->prev_stage = rc->stages->collection[i - 1];
      else
         stage->prev_stage = NULL;
   }
}

//
// rc_build()
//
void rc_build(render_chain_t* rc)
{
   ASSERT(rc);

   for(size_t i = 0; i < rc->stages->count; i++)
   {
      render_stage_t* stage = rc->stages->collection[i];
      if(stage->render_mode == RM_GEOMETRY ||
         stage->render_mode == RM_FRAMEBUFFER ||
         stage->render_mode == RM_CUSTOM_FRAMEBUFFER)

         rs_build_tex(stage);
   }
}
