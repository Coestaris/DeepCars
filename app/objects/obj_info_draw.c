//
// Created by maxim on 1/20/20.
//

#ifdef __GNUC__
#pragma implementation "obj_info_draw.h"
#endif
#include "obj_info_draw.h"
#include "../rendering/renderer.h"
#include "../rendering/text_rendering.h"
#include <stdlib.h>
#include <stdatomic.h>

static font_data_t* font_data;
static font_data_t* sec_font_data;

static char buff[10][70];
static float last_fps;
static bool draw_text;

void switch_text_drawer(void)
{
   draw_text = !draw_text;
}

void update_drawer(object_t* this)
{
   if(draw_text)
   {
      list_t* objects = u_get_objects();
      render_chain_t* chain = rc_get_current();
      float fps = u_get_fps();

      if (fps != last_fps)
      {
         snprintf(buff[0], sizeof(buff[0]), "%s. FPS: %.3f (%i objects)\n",
                  get_stage_string(), (float) fps, (int) objects->count);

         for (size_t i = 0; i < chain->stages->count; i++)
         {
            render_stage_t* stage = chain->stages->collection[i];
            snprintf(buff[i + 1], sizeof(buff[i + 1]), "%i. %s: %.3f ms",
                     (int) i, stage->name, stage->render_time);
         }
      }
      last_fps = fps;

      for (size_t i = 0; i < chain->stages->count + 1; i++)
         draw_monospace_string(0, i == 0 ? font_data : sec_font_data,
                               vec2f(0, 16 * i + (i != 0 ? 15 : 0)), i == 0 ? vec2f(0.5, 0.5) : vec2f(0.3, 0.3),
                               buff[i]);
   }
}

object_t* create_info_drawer()
{
   draw_text = true;
   font_data = DEEPCARS_MALLOC(sizeof(font_data_t));
   font_data->color = COLOR_BLACK;
   font_data->color_off = 0.1f;
   font_data->color_k = 9.0f;
   //font_da->a.color[3] = 1;

   font_data->border_color = COLOR_WHITE;
   font_data->back_off = 0.5f;
   font_data->back_k = 4.5f;
   //font_data.border_color[3] = 1;

   sec_font_data = DEEPCARS_MALLOC(sizeof(font_data_t));
   sec_font_data->color = COLOR_BLACK;
   sec_font_data->color_off = 0.1f;
   sec_font_data->color_k = 9.0f;

   sec_font_data->border_color = COLOR_WHITE;
   sec_font_data->back_off = 0.5f;
   sec_font_data->back_k = 4.5f;
   //sec_font_data.border_color[3] = 1;

   object_t* this = o_create();
   this->update_func = update_drawer;
   return this;
}
