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

static bool free_regions = false;
static bool about        = false;

static blurred_region_t* btn_run_br   = NULL;
static blurred_region_t* btn_about_br = NULL;
static blurred_region_t* btn_exit_br  = NULL;

static blurred_region_t* btn_back_br = NULL;
static blurred_region_t* about_page  = NULL;

static texture_t* logo_texture       = NULL;
static texture_t* about_logo_texture = NULL;
static texture_t* black_texture      = NULL;

static vec4  selected_color = NULL;
static vec4  default_color  = NULL;

static bool  changing_trans = false;
static bool  exiting        = false;
static bool  going_editor   = false;
static float p = 1;
static float t = 1;

static float sprite_transparency;

static bool in_rec(blurred_region_t* br, vec2 pos)
{
   return
      pos.x > br->x && pos.x < br->x + br->w &&
      pos.y > br->y && pos.y < br->y + br->h;
}

static void update_menu_drawer(object_t* this)
{
   vec2 mouse = u_get_mouse_pos();

   if(exiting || going_editor)
   {
      p = 1 - smootherstep(0,0, t += exiting ? 0.03f : 0.02f);
      gr_pq_push_sprite(4, black_texture,
                        vec2e,
                        vec2f(default_win->w,default_win->h), vec2e, 0, default_sprite_renderer, &p);

      if(p <= 0)
      {
         if(exiting)
            u_close();
         else
         {
            rc_set_current(editor_rc);
            scm_load_scene(SCENEID_EDITOR, true);
         }
         return;
      }
   }

   if((!about && ((!changing_trans) || (changing_trans && t != 0))) || (about && changing_trans && p > 0))
      gr_pq_push_sprite(0, logo_texture,
            vec2f(((float)win->w - (float)logo_texture->width) / 2.0f, 75),
            vec2u, vec2e, 0, default_sprite_renderer, &sprite_transparency);
   else
      gr_pq_push_sprite(0, about_logo_texture,
                        vec2f(((float)win->w - (float)about_logo_texture->width) / 2.0f, 75),
                        vec2u, vec2e, 0, default_sprite_renderer, &sprite_transparency);

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

   float delta = (float)u_get_delta();
   if(changing_trans)
   {
      if((p -= 0.025f * delta) < 0)
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

         if((t += 0.025f * delta) > 1)
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

static void mouse_menu_drawer(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && !exiting && !going_editor)
   {
      vec2 pos = vec2f(x, y);
      if(!about)
      {
         if (in_rec(btn_run_br, pos))
         {
            p = 1;
            t = 0;
            going_editor = true;
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
            p = 1;
            t = 0;
            exiting = true;
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

static void free_menu_drawer(object_t* drawer)
{
   if(!free_regions)
   {
      vec4_free(selected_color);
      vec4_free(default_color);

      free_br(btn_run_br);
      free_br(btn_about_br);
      free_br(btn_exit_br);

      free_br(btn_back_br);
      free_br(about_page);
      free_regions = true;
      blurred_regions->count = 0;
   }
}

object_t* create_menu_drawer()
{
   black_texture = rm_getn(TEXTURE, "__generated_mt_grass_trans_0.0_0.0_0.0");
   free_regions = false;
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
