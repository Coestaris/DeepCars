//
// Created by maxim on 1/26/20.
//

#ifdef __GNUC__
#pragma implementation "menu_drawert.h"
#endif
#include "obj_menu_drawer.h"
#include "../../rendering/vfx.h"
#include "../../win_defaults.h"
#include "../../../lib/resources/rmanager.h"
#include "../../rendering/renderer.h"

bool about;
blurred_region_t* btn_run_br;
blurred_region_t* btn_about_br;
blurred_region_t* btn_exit_br;
texture_t* logo_texture;

vec4 selected_color;
vec4 default_color;

bool in_rec(blurred_region_t* br, vec2f_t pos)
{
   return
      pos.x > br->x && pos.x < br->x + br->w &&
      pos.y > br->y && pos.y < br->y + br->h;
}

void update_menu_drawer(object_t* this)
{
   vec2f_t mouse = u_get_mouse_pos();

   btn_run_br->gray_color = in_rec(btn_run_br, mouse) ? selected_color : default_color;
   btn_about_br->gray_color = in_rec(btn_about_br, mouse) ? selected_color : default_color;
   btn_exit_br->gray_color = in_rec(btn_exit_br, mouse) ? selected_color : default_color;

   gr_pq_push_sprite(0, logo_texture,
         vec2f(((float)win->w - (float)logo_texture->width) / 2.0f, 75),
         vec2f(1,1), vec2f(0, 0), 0, true, NULL);
}

void mouse_menu_drawer(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
   {
      vec2f_t pos = vec2f(x, y);
      if(in_rec(btn_run_br, pos)) {
         puts("RUN");
      } else if(in_rec(btn_about_br, pos)) {
         puts("ABOUT");
      } else if(in_rec(btn_exit_br, pos)) {
         u_close();
      }
   }
}

object_t* create_menu_drawer()
{
   object_t* this = o_create();
   render_stage_t* rs = rc->stages->collection[STAGE_SHADING];

   btn_run_br = create_br(win, vec2f(419, 250),
         vec2f(359, 91), 1, vec2f(win->w, win->h));
   btn_run_br->tex = rm_getn(TEXTURE, "btn_run_editor");
   btn_run_br->back_tex = rs->color0_tex;

   btn_about_br = create_br(win, vec2f(419, 391),
         vec2f(359, 91), 1, vec2f(win->w, win->h));
   btn_about_br->tex = rm_getn(TEXTURE, "btn_about");
   btn_about_br->back_tex = rs->color0_tex;

   btn_exit_br = create_br(win, vec2f(419, 530),
         vec2f(359, 91), 1, vec2f(win->w, win->h));
   btn_exit_br->tex = rm_getn(TEXTURE, "btn_exit");
   btn_exit_br->back_tex = rs->color0_tex;

   logo_texture = rm_getn(TEXTURE, "logo");

   list_push(blurred_regions, btn_run_br);
   list_push(blurred_regions, btn_about_br);
   list_push(blurred_regions, btn_exit_br);

   this->update_func = update_menu_drawer;
   this->mouse_event_func = mouse_menu_drawer;

   selected_color = cvec4(0.5, 0.55, 0.5, 1);
   default_color = cvec4(0.3, 0.3, 0.3, 1);

   return this;
}


