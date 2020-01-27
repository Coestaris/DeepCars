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
#include "obj_menu_camera_mover.h"

bool about;
blurred_region_t* btn_run_br;
blurred_region_t* btn_about_br;
blurred_region_t* btn_exit_br;

blurred_region_t* btn_back_br;
blurred_region_t* about_page;

texture_t* logo_texture;
texture_t* about_logo_texture;

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

   if(!about)
   {
      btn_run_br->gray_color = in_rec(btn_run_br, mouse) ? selected_color : default_color;
      btn_about_br->gray_color = in_rec(btn_about_br, mouse) ? selected_color : default_color;
      btn_exit_br->gray_color = in_rec(btn_exit_br, mouse) ? selected_color : default_color;

      gr_pq_push_sprite(0, logo_texture,
            vec2f(((float)win->w - (float)logo_texture->width) / 2.0f, 75),
            vec2f(1,1), vec2f(0, 0), 0, true, NULL);
   }
   else
   {
      btn_back_br->gray_color = in_rec(btn_back_br, mouse) ? selected_color : default_color;

      gr_pq_push_sprite(0, about_logo_texture,
                        vec2f(((float)win->w - (float)about_logo_texture->width) / 2.0f, 75),
                        vec2f(1,1), vec2f(0, 0), 0, true, NULL);
   }
}

void mouse_menu_drawer(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
   {
      vec2f_t pos = vec2f(x, y);
      if(!about)
      {
         if (in_rec(btn_run_br, pos))
         {
            puts("RUN");
         }
         else if (in_rec(btn_about_br, pos))
         {
            about = true;
            btn_run_br->visible = false;
            btn_about_br->visible = false;
            btn_exit_br->visible = false;

            btn_back_br->visible = true;
            about_page->visible = true;
            update_camera(20, 40);
         }
         else if (in_rec(btn_exit_br, pos))
         {
            u_close();
         }
      }
      else
      {
         if(in_rec(btn_back_br, pos))
         {
            about = false;
            btn_run_br->visible = true;
            btn_about_br->visible = true;
            btn_exit_br->visible = true;

            btn_back_br->visible = false;
            about_page->visible = false;
            update_camera(default_camera_y, default_camera_r);
         }
      }
   }
}

object_t* create_menu_drawer()
{
   about = false;
   object_t* this = o_create();
   render_stage_t* rs = rc->stages->collection[STAGE_SHADING];

   selected_color = cvec4(0.5, 0.55, 0.5, 1);
   default_color = cvec4(0.3, 0.3, 0.3, 1);

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

   btn_back_br = create_br(win, vec2f(419, 640),
                           vec2f(359, 91), 1, vec2f(win->w, win->h));
   btn_back_br->tex = rm_getn(TEXTURE, "btn_back");
   btn_back_br->back_tex = rs->color0_tex;
   btn_back_br->visible = false;

   about_page = create_br(win, vec2f(266, 173),
                           vec2f(673, 449), 1, vec2f(win->w, win->h));
   about_page->tex = rm_getn(TEXTURE, "about");
   about_page->back_tex = rs->color0_tex;
   about_page->gray_color = default_color;
   about_page->visible = false;

   logo_texture = rm_getn(TEXTURE, "logo");
   about_logo_texture = rm_getn(TEXTURE, "about_logo");

   list_push(blurred_regions, btn_back_br);

   list_push(blurred_regions, btn_run_br);
   list_push(blurred_regions, btn_about_br);
   list_push(blurred_regions, btn_exit_br);

   list_push(blurred_regions, about_page);

   this->update_func = update_menu_drawer;
   this->mouse_event_func = mouse_menu_drawer;

   return this;
}


