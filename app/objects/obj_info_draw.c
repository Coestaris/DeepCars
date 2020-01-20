//
// Created by maxim on 1/20/20.
//

#ifdef __GNUC__
#pragma implementation "obj_info_draw.h"
#endif
#include "obj_info_draw.h"
#include "../rendering/renderer.h"
#include <stdlib.h>

char buff[10][50];
float last_fps;

void update_drawer(object_t* this)
{
   list_t* objects = u_get_objects();
   render_chain_t* chain = rc_get_current();
   float fps = u_get_fps();

   if(fps != last_fps)
   {
      snprintf(buff[0], sizeof(buff[0]), "%s. FPS: %.3f (%i objects)\n",
               get_ssao_stage_string(), (float)fps, (int)objects->count);

      for(size_t i = 0; i < chain->stages->count; i++)
      {
         render_stage_t* stage = chain->stages->collection[i];
         snprintf(buff[i + 1], sizeof(buff[i + 1]), "%i. %s: %f ms",
                  (int)i, stage->name, stage->render_time);
      }
   }
   last_fps = fps;

   for(size_t i = 0; i < chain->stages->count + 1; i++)
      draw_default_string(0, vec2f(0,24 * i), vec2f(.5,.5),
            0.5, 9, COLOR_WHITE, buff[i]);
}

object_t* create_info_drawer()
{
   object_t* this = o_create();
   this->draw_info->drawable = false;
   this->update_func = update_drawer;
   return this;
}
