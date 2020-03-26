//
// Created by maxim on 2/18/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_drawer.h"
#endif
#include "obj_editor_drawer.h"

#include "obj_editor_map.h"
#include "../../win_defaults.h"
#include "../../../lib/resources/rmanager.h"
#include "../../rendering/renderer.h"
#include "map_saver.h"
#include "../../../osdialog/osdialog.h"

#define TOOLBAR_SIZE 33
#define GRID_TEXTURE_SIZE 256

#define CHECK(position)                                       \
   pos.x > position.x && pos.x < position.x + TOOLBAR_SIZE && \
   pos.y > position.y && pos.y < position.y + TOOLBAR_SIZE    \

#define CHECK_SIZE(position, tex)                            \
   pos.x > position.x && pos.x < position.x + tex->width &&  \
   pos.y > position.y && pos.y < position.y + tex->height    \

static texture_t* run_button_texture[2]    = {NULL, NULL};
static texture_t* toolbar_eraser_texture   = NULL;
static texture_t* toolbar_grid_texture[5]  = {NULL, NULL, NULL, NULL, NULL};
static texture_t* toolbar_obstacle_texture = NULL;
static texture_t* toolbar_slip_texture     = NULL;
static texture_t* toolbar_start_texture    = NULL;
static texture_t* toolbar_fin_texture      = NULL;
static texture_t* toolbar_wall_texture     = NULL;

static texture_t* toolbar_selected      = NULL;
static texture_t* toolbar_frame         = NULL;
static texture_t* toolbar_back          = NULL;
static texture_t* toolbar_back_selected = NULL;
static texture_t* toolbar_clicked       = NULL;

static texture_t* tabbutton_arrow       = NULL;
static texture_t* tabbutton_ffnn        = NULL;
static texture_t* tabbutton_ga          = NULL;
static texture_t* tabbutton_map         = NULL;
static texture_t* tabbutton_file        = NULL;

static vec2 tabbutton_ffnn_pos     = vec2e;
static vec2 tabbutton_ga_pos       = vec2e;
static vec2 tabbutton_map_pos      = vec2e;
static vec2 tabbutton_file_pos     = vec2e;
static vec2 tabbutton_selected_pos = vec2e;

static texture_t* grid_texture         = NULL;
static texture_t* editor_black_texture = NULL;

static vec2 run_button_pos        = vec2e;
static vec2 toolbar_eraser_pos    = vec2e;
static vec2 toolbar_grid_pos      = vec2e;
static vec2 toolbar_obstacle_pos  = vec2e;
static vec2 toolbar_slip_pos      = vec2e;
static vec2 toolbar_start_pos     = vec2e;
static vec2 toolbar_fin_pos       = vec2e;
static vec2 toolbar_wall_pos      = vec2e;

static vec2       tab_file_save_pos = vec2e;
static vec2       tab_file_load_pos = vec2e;
static texture_t* tab_file_selected = NULL;
static texture_t* tab_file_clicked  = NULL;

static bool     toolbar_eraser_clicked   = false;
static bool     toolbar_grid_clicked     = false;
static bool     toolbar_obstacle_clicked = false;
static bool     toolbar_slip_clicked     = false;
static bool     toolbar_start_clicked    = false;
static bool     toolbar_fin_clicked      = false;
static bool     toolbar_wall_clicked     = false;

static texture_t* tab_texture        = NULL;
static texture_t* tab_file_texture   = NULL;
static texture_t* tab_ffnn_texture   = NULL;
static texture_t* tab_ga_texture     = NULL;
static texture_t* tab_map_texture    = NULL;
static vec2       tab_pos            = vec2e;
static float      editor_p           = 0;
static osdialog_filters* filters     = NULL;

static vec2 selected_toolbar_state_pos = vec2e;

enum _toolbar_state toolbar_state     = WALL;
enum _tab_state     tab_state         = TAB_CLOSED;
uint8_t             grid_state        = EDITOR_GRID_START_STATE;
uint8_t             current_grid_size = EDITOR_GRID_START;

void (*field_click_func)(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);

static void create_grid(size_t size)
{
   char buff[256];
   snprintf(buff, sizeof(buff), "__%li_grid", size);

   grid_texture = rm_getn_try(TEXTURE, buff);
   if(!grid_texture)
   {
      pngImage* image = oilCreateImg();
      image->width = GRID_TEXTURE_SIZE;
      image->height = GRID_TEXTURE_SIZE;
      image->colorMatrix = oilColorMatrixAlloc(true, GRID_TEXTURE_SIZE, GRID_TEXTURE_SIZE);

      oilColor grid_color = EDITOR_GRID_COLOR;
      oilColor grid_color_accent = EDITOR_GRID_COLOR_ACCENT;

      for(size_t x = 0; x < GRID_TEXTURE_SIZE / size; x++)
         oilGrDrawLine(image->colorMatrix, x * size, 0, x * size, GRID_TEXTURE_SIZE - 1,
               x % 2 ? grid_color : grid_color_accent);

      for(size_t y = 0; y < GRID_TEXTURE_SIZE / size; y++)
         oilGrDrawLine(image->colorMatrix, 0, y * size, GRID_TEXTURE_SIZE - 1, y * size,
               y % 2 ? grid_color : grid_color_accent);

      imageData* data = oilGetPNGImageData(image, GL_RGBA, GL_UNSIGNED_BYTE);
      if(!data)
      {
         oilPrintError();
         abort();
      }

      GLuint id = oilGetTexture(data, GL_CLAMP_TO_BORDER, GL_NEAREST, GL_NEAREST, NULL);
      if(!id)
      {
         oilPrintError();
         abort();
      }

      texture_t* texture = t_create(strdup(buff));
      texture->texID = id;
      texture->width = GRID_TEXTURE_SIZE;
      texture->height = GRID_TEXTURE_SIZE;
      texture->type = GL_TEXTURE_2D;

      rm_push(TEXTURE, texture, -1);

      grid_texture = texture;

      oilFreeImageData(data);
      oilPNGFreeImage(image);
   }
}

static void draw(texture_t* texture, vec2 position)
{
   static float a = 0;
   gr_pq_push_sprite(EDITOR_MAIN_DEPTH, texture, position, vec2u,
                     vec2e, 0, default_sprite_renderer, &a);
}

static void draw_depth(size_t depth, texture_t* texture, vec2 position)
{
   static float a = 0;
   gr_pq_push_sprite(depth, texture, position, vec2u,
                     vec2e, 0, default_sprite_renderer, &a);
}

static void draw_centered(texture_t* texture, vec2 position)
{
   static float a = 0;
   gr_pq_push_sprite(EDITOR_MAIN_DEPTH, texture,
         vec2f(position.x - (texture->width - TOOLBAR_SIZE) / 2.0,
               position.y - (texture->height - TOOLBAR_SIZE) / 2.0),
         vec2u,
         vec2e, 0, default_sprite_renderer, &a);
}

static void update_editor(object_t* this)
{
   if(editor_p <= 1)
   {
      editor_p += 0.03f;
      gr_pq_push_sprite(EDITOR_MENU_DEPTH, editor_black_texture,
                        vec2e,
                        vec2f(default_win->w,default_win->h), vec2e, 0, default_sprite_renderer, &editor_p);
   }

   vec2 pos = u_get_mouse_pos();
   int32_t ms = u_get_mouse_state(MOUSE_LEFT);

   if(CHECK_SIZE(run_button_pos, run_button_texture[1])) draw(run_button_texture[1], run_button_pos);
   else draw(run_button_texture[0], run_button_pos);

   if(CHECK(toolbar_eraser_pos))
   {
      draw(toolbar_back_selected, toolbar_eraser_pos);
      if(toolbar_eraser_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_eraser_pos);
   }
   else draw(toolbar_back, toolbar_eraser_pos);

   if(CHECK(toolbar_grid_pos))
   {
      draw(toolbar_back_selected, toolbar_grid_pos);
      if(toolbar_grid_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_grid_pos);
   }
   else draw(toolbar_back, toolbar_grid_pos);

   if(CHECK(toolbar_wall_pos))
   {
      draw(toolbar_back_selected, toolbar_wall_pos);
      if(toolbar_wall_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_wall_pos);
   }
   else draw(toolbar_back, toolbar_wall_pos);

   if(CHECK(toolbar_start_pos))
   {
      draw(toolbar_back_selected, toolbar_start_pos);
      if(toolbar_start_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_start_pos);
   }
   else draw(toolbar_back, toolbar_start_pos);

   if(CHECK(toolbar_fin_pos))
   {
      draw(toolbar_back_selected, toolbar_fin_pos);
      if(toolbar_fin_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_fin_pos);
   }
   else draw(toolbar_back, toolbar_fin_pos);

   if(CHECK(toolbar_obstacle_pos))
   {
      draw(toolbar_back_selected, toolbar_obstacle_pos);
      if(toolbar_obstacle_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_obstacle_pos);
   }
   else draw(toolbar_back, toolbar_obstacle_pos);

   if(CHECK(toolbar_slip_pos))
   {
      draw(toolbar_back_selected, toolbar_slip_pos);
      if(toolbar_slip_clicked)
         draw_depth(EDITOR_MENU_DEPTH, toolbar_clicked, toolbar_slip_pos);
   }
   else draw(toolbar_back, toolbar_slip_pos);

   draw_centered(toolbar_eraser_texture, toolbar_eraser_pos);
   draw_centered(toolbar_frame, toolbar_eraser_pos);

   draw_centered(toolbar_grid_texture[grid_state], toolbar_grid_pos);
   draw_centered(toolbar_frame, toolbar_grid_pos);

   draw_centered(toolbar_wall_texture, toolbar_wall_pos);
   draw_centered(toolbar_frame, toolbar_wall_pos);

   draw_centered(toolbar_start_texture, toolbar_start_pos);
   draw_centered(toolbar_frame, toolbar_start_pos);

   draw_centered(toolbar_fin_texture, toolbar_fin_pos);
   draw_centered(toolbar_frame, toolbar_fin_pos);

   draw_centered(toolbar_obstacle_texture, toolbar_obstacle_pos);
   draw_centered(toolbar_frame, toolbar_obstacle_pos);

   draw_centered(toolbar_slip_texture, toolbar_slip_pos);
   draw_centered(toolbar_frame, toolbar_slip_pos);

   draw(tabbutton_ffnn, tabbutton_ffnn_pos);
   draw(tabbutton_ga, tabbutton_ga_pos);
   draw(tabbutton_map, tabbutton_map_pos);
   draw(tabbutton_file, tabbutton_file_pos);
   if(tab_state != TAB_CLOSED)
   {
      draw(tabbutton_arrow, tabbutton_selected_pos);
      draw(tab_texture, tab_pos);
      switch(tab_state)
      {
         case TAB_FFNN: if(tab_ffnn_texture) draw(tab_ffnn_texture, tab_pos); break;
         case TAB_GA: if(tab_ga_texture) draw(tab_ga_texture, tab_pos); break;
         case TAB_MAP: if(tab_map_texture) draw(tab_map_texture, tab_pos); break;
         case TAB_FILE: if(tab_file_texture) draw(tab_file_texture, tab_pos); break;
         default:
            break;
      }
   }
   draw(toolbar_selected, selected_toolbar_state_pos);

   if(CHECK_SIZE(tab_file_load_pos, tab_file_selected) && tab_state == TAB_FILE)
   {
      draw(ms == MOUSE_PRESSED ? tab_file_clicked : tab_file_selected, tab_file_load_pos);
   }
   if(CHECK_SIZE(tab_file_save_pos, tab_file_selected) && tab_state == TAB_FILE)
   {
      draw(ms == MOUSE_PRESSED ? tab_file_clicked : tab_file_selected, tab_file_save_pos);
   }

   if(grid_texture)
   {
      for(size_t x = 0; x < (size_t)(default_win->w / GRID_TEXTURE_SIZE + 1); x++)
      for(size_t y = 0; y < (size_t)(default_win->h / GRID_TEXTURE_SIZE + 1); y++)
      {
         float screen_x = x * (float)GRID_TEXTURE_SIZE;
         float screen_y = y * (float)GRID_TEXTURE_SIZE;

         draw_depth(EDITOR_GRID_DEPTH, grid_texture, vec2f(screen_x, screen_y));
      }
   }
}

static void mouse_editor(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   vec2 pos = u_get_mouse_pos();

   if(CHECK(toolbar_eraser_pos))
   {
      toolbar_eraser_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = ERASER;
         selected_toolbar_state_pos = toolbar_eraser_pos;
      }
   }
   else if(CHECK(toolbar_grid_pos))
   {
      toolbar_grid_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         grid_state = (grid_state + 1) % (EDITOR_GRID_STATES - 1);
         switch(grid_state)
         {
            case 0: current_grid_size = EDITOR_GRID_SIZE_1; break;
            case 1: current_grid_size = EDITOR_GRID_SIZE_2; break;
            case 2: current_grid_size = EDITOR_GRID_SIZE_3; break;
            case 3: current_grid_size = EDITOR_GRID_SIZE_4; break;
            default:
            case 4:
               current_grid_size = EDITOR_GRID_SIZE_0; break;
         }

         if(current_grid_size)
            create_grid(current_grid_size);
         else
            grid_texture = NULL;
      }
   }
   else if(CHECK(toolbar_obstacle_pos))
   {
      toolbar_obstacle_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = OBSTACLE;
         selected_toolbar_state_pos = toolbar_obstacle_pos;
      }
   }
   else if(CHECK(toolbar_slip_pos))
   {
      toolbar_slip_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = ERASER;
         selected_toolbar_state_pos = toolbar_slip_pos;
      }
   }
   else if(CHECK(toolbar_start_pos))
   {
      toolbar_start_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = START;
         selected_toolbar_state_pos = toolbar_start_pos;
      }
   }
   else if(CHECK(toolbar_fin_pos))
   {
      toolbar_fin_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = FIN;
         selected_toolbar_state_pos = toolbar_fin_pos;
      }
   }
   else if(CHECK(toolbar_wall_pos))
   {
      toolbar_wall_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = WALL;
         selected_toolbar_state_pos = toolbar_wall_pos;
      }
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && CHECK_SIZE(tabbutton_ffnn_pos, tabbutton_ffnn))
   {
      if(tab_state == TAB_FFNN) tab_state = TAB_CLOSED;
      else
      {
         tabbutton_selected_pos = tabbutton_ffnn_pos;
         tab_state = TAB_FFNN;
      }
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && CHECK_SIZE(tabbutton_ga_pos, tabbutton_ga))
   {
      if(tab_state == TAB_GA) tab_state = TAB_CLOSED;
      else
      {
         tabbutton_selected_pos = tabbutton_ga_pos;
         tab_state = TAB_GA;
      }
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && CHECK_SIZE(tabbutton_map_pos, tabbutton_map))
   {
      if(tab_state == TAB_MAP) tab_state = TAB_CLOSED;
      else
      {
         tabbutton_selected_pos = tabbutton_map_pos;
         tab_state = TAB_MAP;
      }
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && CHECK_SIZE(tabbutton_file_pos, tabbutton_file))
   {
      if(tab_state == TAB_FILE) tab_state = TAB_CLOSED;
      else
      {
         tabbutton_selected_pos = tabbutton_file_pos;
         tab_state = TAB_FILE;
      }
   }
   else if(tab_state != TAB_CLOSED && state == MOUSE_RELEASE && mouse == MOUSE_LEFT && CHECK_SIZE(tab_pos, tab_texture))
   {
      if(CHECK_SIZE(tab_file_load_pos, tab_file_selected))
      {
         char* fn = osdialog_file(OSDIALOG_SAVE, MAP_SAVE_DIR, EDITOR_DEFAULT_MAP_FN, filters);
         if(fn)
         {
            map_save(walls, map_objects, fn, prev_point, first_point_set);
            DEEPCARS_FREE(fn);
         }
      }
      else if(CHECK_SIZE(tab_file_save_pos, tab_file_selected))
      {
         char* fn = osdialog_file(OSDIALOG_OPEN, MAP_SAVE_DIR, EDITOR_DEFAULT_MAP_FN, filters);
         if(fn)
         {
            map_load(walls, map_objects, fn, &prev_point, &first_point_set);
            DEEPCARS_FREE(fn);
         }
      }
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && CHECK_SIZE(run_button_pos, run_button_texture[0]))
   {
      rc_set_current(default_rc);
      scm_load_scene(SCENEID_GAME, true);

      update_lights();
      update_shadow_light();
   }
   else
   {
      field_click_func(x, y, state, mouse);
   }
}

object_t* create_editor_drawer(void)
{
   if(!filters)
      filters = osdialog_filters_parse(EDITOR_FILE_FILTER);

   tab_texture      = rm_getn(TEXTURE, "editor_tab");
   tab_file_texture = rm_getn(TEXTURE, "editor_tab_file");
   tab_ffnn_texture = NULL; // rm_getn(TEXTURE, "editor_tab_ffnn");
   tab_ga_texture   = NULL; // rm_getn(TEXTURE, "editor_tab_ga");
   tab_map_texture  = NULL; // rm_getn(TEXTURE, "editor_tab_map");

   tab_file_save_pos = vec2f(EDITOR_MENU_TAB_FILE_SAVE_POS);
   tab_file_load_pos = vec2f(EDITOR_MENU_TAB_FILE_LOAD_POS);
   tab_file_selected = rm_getn(TEXTURE, "editor_tab_file_selected");
   tab_file_clicked  = rm_getn(TEXTURE, "editor_tab_file_clicked");

   editor_black_texture = rm_getn(TEXTURE, "__generated_mt_grass_trans_0.0_0.0_0.0");

   tab_pos              = vec2f(EDITOR_MENU_TAB_POS);
   run_button_pos       = vec2f(EDITOR_MENU_RUNBTN_POS);
   toolbar_eraser_pos   = vec2f(EDITOR_MENU_ERASER_POS);
   toolbar_grid_pos     = vec2f(EDITOR_MENU_GRID_POS);
   toolbar_wall_pos     = vec2f(EDITOR_MENU_WALL_POS);
   toolbar_start_pos    = vec2f(EDITOR_MENU_START_POS);
   toolbar_fin_pos      = vec2f(EDITOR_MENU_FIN_POS);
   toolbar_obstacle_pos = vec2f(EDITOR_MENU_OBSTACLE_POS);
   toolbar_slip_pos     = vec2f(EDITOR_MENU_SLIP_POS);

   create_grid(EDITOR_GRID_START);
   grid_state        = EDITOR_GRID_START_STATE;
   current_grid_size = EDITOR_GRID_START;

   tabbutton_arrow   = rm_getn(TEXTURE, "editor_tabbutton_arrow");
   tabbutton_ffnn    = rm_getn(TEXTURE, "editor_tabbutton_ffnn");
   tabbutton_ga      = rm_getn(TEXTURE, "editor_tabbutton_ga");
   tabbutton_map     = rm_getn(TEXTURE, "editor_tabbutton_map");
   tabbutton_file    = rm_getn(TEXTURE, "editor_tabbutton_file");

   tabbutton_ffnn_pos = vec2f(EDITOR_MENU_TABBUTTON_FFNN_POS);
   tabbutton_ga_pos   = vec2f(EDITOR_MENU_TABBUTTON_GA_POS);
   tabbutton_map_pos  = vec2f(EDITOR_MENU_TABBUTTON_MAP_POS);
   tabbutton_file_pos = vec2f(EDITOR_MENU_TABBUTTON_FILE_POS);
   tab_state = TAB_CLOSED;

   toolbar_selected      = rm_getn(TEXTURE, "editor_toolbar_selected");
   toolbar_frame         = rm_getn(TEXTURE, "editor_toolbar_frame");
   toolbar_back          = rm_getn(TEXTURE, "editor_toolbar_frame_back");
   toolbar_back          = rm_getn(TEXTURE, "editor_toolbar_frame_back");
   toolbar_back_selected = rm_getn(TEXTURE, "editor_toolbar_frame_back_selected");
   toolbar_clicked       = rm_getn(TEXTURE, "editor_toolbar_frame_clicked");

   run_button_texture[0] = rm_getn(TEXTURE, "editor_run");
   run_button_texture[1] = rm_getn(TEXTURE, "editor_run_selected");

   toolbar_grid_texture[0] = rm_getn(TEXTURE, "editor_toolbar_grid_8");
   toolbar_grid_texture[1] = rm_getn(TEXTURE, "editor_toolbar_grid_16");
   toolbar_grid_texture[2] = rm_getn(TEXTURE, "editor_toolbar_grid_32");
   toolbar_grid_texture[3] = rm_getn(TEXTURE, "editor_toolbar_grid_128");
   toolbar_grid_texture[4] = rm_getn(TEXTURE, "editor_toolbar_grid_no");

   toolbar_eraser_texture   = rm_getn(TEXTURE, "editor_toolbar_eraser");
   toolbar_wall_texture     = rm_getn(TEXTURE, "editor_toolbar_wall");
   toolbar_start_texture    = rm_getn(TEXTURE, "editor_toolbar_start");
   toolbar_fin_texture      = rm_getn(TEXTURE, "editor_toolbar_finish");
   toolbar_obstacle_texture = rm_getn(TEXTURE, "editor_toolbar_obstacle");
   toolbar_slip_texture     = rm_getn(TEXTURE, "editor_toolbar_slip");

   toolbar_state = WALL;
   selected_toolbar_state_pos = toolbar_wall_pos;

   object_t* this = o_create();
   this->update_func = update_editor;
   this->mouse_event_func = mouse_editor;

   return this;
}


