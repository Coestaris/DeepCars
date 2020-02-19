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

bool freed_regions;
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
bool changing_trans;
float p = 1;
float t = 1;
float sprite_transparency;

bool in_rec(blurred_region_t* br, vec2f_t pos)
{
   return
      pos.x > br->x && pos.x < br->x + br->w &&
      pos.y > br->y && pos.y < br->y + br->h;
}

void update_menu_drawer(object_t* this)
{
   vec2f_t mouse = u_get_mouse_pos();

   if((!about && ((!changing_trans) || (changing_trans && t != 0))) || (about && changing_trans && p > 0))
      gr_pq_push_sprite(0, logo_texture,
            vec2f(((float)win->w - (float)logo_texture->width) / 2.0f, 75),
            vec2f(1,1), vec2f(0, 0), 0, true, &sprite_transparency);
   else
      gr_pq_push_sprite(0, about_logo_texture,
                        vec2f(((float)win->w - (float)about_logo_texture->width) / 2.0f, 75),
                        vec2f(1,1), vec2f(0, 0), 0, true, &sprite_transparency);

   if(!about)
   {
      btn_run_br->gray_color = in_rec(btn_run_br, mouse) ? selected_color : default_color;
      btn_about_br->gray_color = in_rec(btn_about_br, mouse) ? selected_color : default_color;
      btn_exit_br->gray_color = in_rec(btn_exit_br, mouse) ? selected_color : default_color;

   }
   else
   {
      btn_back_br->gray_color = in_rec(btn_back_br, mouse) ? selected_color : default_color;
   }

   if(changing_trans)
   {
      if((p -= 0.025f) < 0)
      {
         sprite_transparency = 1 - t;
         if(about)
         {
            btn_run_br->visible = false;
            btn_about_br->visible = false;
            btn_exit_br->visible = false;

            btn_back_br->visible = true;
            about_page->visible = true;
            btn_back_br->transparency = t;
            about_page->transparency = t;
         }
         else
         {
            btn_back_br->visible = false;
            about_page->visible = false;

            btn_run_br->visible = true;
            btn_about_br->visible = true;
            btn_exit_br->visible = true;
            btn_run_br->transparency = t;
            btn_about_br->transparency = t;
            btn_exit_br->transparency = t;
         }

         if((t += 0.025f) > 1)
            changing_trans = false;
      }
      else
      {
         sprite_transparency = 1 - p;
         if(about)
         {
            btn_run_br->transparency = p;
            btn_about_br->transparency = p;
            btn_exit_br->transparency = p;
         }
         else
         {
            btn_back_br->transparency = p;
            about_page->transparency = p;
         }
      }
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
            rc_set_current(editor_rc);
            scm_load_scene(SCENEID_EDITOR, true);
         }
         else if (in_rec(btn_about_br, pos))
         {
            about = true;
            changing_trans = true;
            p = 1;
            t = 0;

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
            changing_trans = true;
            p = 1;
            t = 0;
            update_camera(default_camera_y, default_camera_r);
         }
      }
   }
}

void free_menu_drawer(object_t* drawer)
{
   if(!freed_regions)
   {
      vec4_free(selected_color);
      vec4_free(default_color);

      free_br(btn_run_br);
      free_br(btn_about_br);
      free_br(btn_exit_br);

      free_br(btn_back_br);
      free_br(about_page);
      freed_regions = true;
      blurred_regions->count = 0;
   }
}

object_t* create_menu_drawer()
{
   freed_regions = false;
   about = false;
   object_t* this = o_create();
   render_stage_t* rs = default_rc->stages->collection[STAGE_SHADING];

   selected_color = cvec4(0.5, 0.55, 0.5, 1);
   default_color = cvec4(0.3, 0.3, 0.3, 1);

   btn_run_br = create_br(win, vec2f(419, 250),
         vec2f(359, 91), vec2f(win->w, win->h));
   btn_run_br->tex = rm_getn(TEXTURE, "btn_run_editor");
   btn_run_br->back_tex = rs->color0_tex;

   btn_about_br = create_br(win, vec2f(419, 391),
         vec2f(359, 91), vec2f(win->w, win->h));
   btn_about_br->tex = rm_getn(TEXTURE, "btn_about");
   btn_about_br->back_tex = rs->color0_tex;

   btn_exit_br = create_br(win, vec2f(419, 530),
         vec2f(359, 91), vec2f(win->w, win->h));
   btn_exit_br->tex = rm_getn(TEXTURE, "btn_exit");
   btn_exit_br->back_tex = rs->color0_tex;

   btn_back_br = create_br(win, vec2f(419, 640),
                           vec2f(359, 91), vec2f(win->w, win->h));
   btn_back_br->tex = rm_getn(TEXTURE, "btn_back");
   btn_back_br->back_tex = rs->color0_tex;
   btn_back_br->visible = false;

   about_page = create_br(win, vec2f(266, 173),
                           vec2f(673, 449), vec2f(win->w, win->h));
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
   this->destroy_func = free_menu_drawer;

   return this;
}
