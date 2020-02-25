//
// Created by maxim on 2/18/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_drawer.h"
#endif
#include "obj_editor_drawer.h"
#include "../../../lib/resources/rmanager.h"
#include "../../rendering/renderer.h"

#define toolbar_size 33
#define grid_size 256

texture_t* run_button_texture[2];
texture_t* toolbar_eraser_texture;
texture_t* toolbar_grid_texture[5];
texture_t* toolbar_obstacle_texture;
texture_t* toolbar_slip_texture;
texture_t* toolbar_start_texture;
texture_t* toolbar_fin_texture;
texture_t* toolbar_wall_texture;

texture_t* toolbar_selected;
texture_t* toolbar_frame;
texture_t* toolbar_back;
texture_t* toolbar_back_selected;
texture_t* toolbar_clicked;

texture_t* tabbutton_ffnn;
texture_t* tabbutton_ga;
texture_t* tabbutton_map;
texture_t* tabbutton_file;

texture_t* grid;
texture_t* editor_black_texture;

vec2f_t run_button_pos;
vec2f_t toolbar_eraser_pos;
vec2f_t toolbar_grid_pos;
vec2f_t toolbar_obstacle_pos;
vec2f_t toolbar_slip_pos;
vec2f_t toolbar_start_pos;
vec2f_t toolbar_fin_pos;
vec2f_t toolbar_wall_pos;

bool toolbar_eraser_clicked;
bool toolbar_grid_clicked;
bool toolbar_obstacle_clicked;
bool toolbar_slip_clicked;
bool toolbar_start_clicked;
bool toolbar_fin_clicked;
bool toolbar_wall_clicked;

float editor_p = 0;

enum _toolbar_state toolbar_state;
vec2f_t selected_toolbar_state_pos;
size_t grid_state;
size_t current_grid_size;
void (*field_click_func)(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);

void create_grid(size_t size)
{
   char buff[256];
   snprintf(buff, sizeof(buff), "__%li_grid", size);

   grid = rm_getn_try(TEXTURE, buff);
   if(!grid)
   {
      pngImage* image = oilCreateImg();
      image->width = grid_size;
      image->height = grid_size;
      image->colorMatrix = oilColorMatrixAlloc(true, grid_size, grid_size);

      oilColor gridColor = {90, 90, 90, 220};
      oilColor gridColor1 = {120, 120, 120, 200};
      for(size_t x = 0; x < grid_size / size; x++)
         oilGrDrawLine(image->colorMatrix, x * size, 0, x * size, grid_size - 1, x % 2 ? gridColor : gridColor1);
      for(size_t y = 0; y < grid_size / size; y++)
         oilGrDrawLine(image->colorMatrix, 0, y * size, grid_size - 1, y * size, y % 2 ? gridColor : gridColor1);

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
      texture->width = grid_size;
      texture->height = grid_size;
      texture->type = GL_TEXTURE_2D;

      rm_push(TEXTURE, texture, -1);

      grid = texture;

      oilFreeImageData(data);
      oilPNGFreeImage(image);
   }
}

void draw(texture_t* texture, vec2f_t position)
{
   float a = 1;
   gr_pq_push_sprite(3, texture, position, vec2f(1, 1),
                     vec2f(0, 0), 0, default_sprite_renderer, &a);
}

void draw_depth(size_t depth, texture_t* texture, vec2f_t position)
{
   float a = 1;
   gr_pq_push_sprite(depth, texture, position, vec2f(1, 1),
                     vec2f(0, 0), 0, default_sprite_renderer, &a);
}

void draw_centered(texture_t* texture, vec2f_t position)
{
   float a = 1;
   gr_pq_push_sprite(3, texture,
         vec2f(position.x - (texture->width - toolbar_size) / 2.0, position.y - (texture->height - toolbar_size) / 2.0),
         vec2f(1, 1),
         vec2f(0, 0), 0, default_sprite_renderer, &a);
}


#define CHECK(position)                           \
   pos.x > position.x && pos.x < position.x + toolbar_size && \
   pos.y > position.y && pos.y < position.y + toolbar_size    \

void update_editor(object_t* this)
{
   if(editor_p <= 1)
   {
      editor_p += 0.03f;
      gr_pq_push_sprite(4, editor_black_texture,
                        vec2f(0, 0),
                        vec2f(default_win->w,default_win->h), vec2f(0, 0), 0, default_sprite_renderer, &editor_p);
   }

   vec2f_t pos = u_get_mouse_pos();

   if(CHECK(run_button_pos)) draw(run_button_texture[1], run_button_pos);
   else draw(run_button_texture[0], run_button_pos);



   if(CHECK(toolbar_eraser_pos))
   {
      draw(toolbar_back_selected, toolbar_eraser_pos);
      if(toolbar_eraser_clicked)
         draw_depth(4, toolbar_clicked, toolbar_eraser_pos);
   }
   else draw(toolbar_back, toolbar_eraser_pos);

   if(CHECK(toolbar_grid_pos))
   {
      draw(toolbar_back_selected, toolbar_grid_pos);
      if(toolbar_grid_clicked)
         draw_depth(4, toolbar_clicked, toolbar_grid_pos);
   }
   else draw(toolbar_back, toolbar_grid_pos);

   if(CHECK(toolbar_wall_pos))
   {
      draw(toolbar_back_selected, toolbar_wall_pos);
      if(toolbar_wall_clicked)
         draw_depth(4, toolbar_clicked, toolbar_wall_pos);
   }
   else draw(toolbar_back, toolbar_wall_pos);

   if(CHECK(toolbar_start_pos))
   {
      draw(toolbar_back_selected, toolbar_start_pos);
      if(toolbar_start_clicked)
         draw_depth(4, toolbar_clicked, toolbar_start_pos);
   }
   else draw(toolbar_back, toolbar_start_pos);

   if(CHECK(toolbar_fin_pos))
   {
      draw(toolbar_back_selected, toolbar_fin_pos);
      if(toolbar_fin_clicked)
         draw_depth(4, toolbar_clicked, toolbar_fin_pos);
   }
   else draw(toolbar_back, toolbar_fin_pos);

   if(CHECK(toolbar_obstacle_pos))
   {
      draw(toolbar_back_selected, toolbar_obstacle_pos);
      if(toolbar_obstacle_clicked)
         draw_depth(4, toolbar_clicked, toolbar_obstacle_pos);
   }
   else draw(toolbar_back, toolbar_obstacle_pos);

   if(CHECK(toolbar_slip_pos))
   {
      draw(toolbar_back_selected, toolbar_slip_pos);
      if(toolbar_slip_clicked)
         draw_depth(4, toolbar_clicked, toolbar_slip_pos);
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



   draw(toolbar_selected, selected_toolbar_state_pos);

   if(grid)
   {
      for(size_t x = 0; x < default_win->w / grid_size + 1; x++)
      for(size_t y = 0; y < default_win->h / grid_size + 1; y++)
      {
         float screen_x = x * (float)grid_size;
         float screen_y = y * (float)grid_size;

         draw_depth(1, grid, vec2f(screen_x, screen_y));
      }
   }
}

void mouse_editor(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   vec2f_t pos = u_get_mouse_pos();

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
         grid_state = (grid_state + 1) % 5;
         switch(grid_state)
         {
            case 0: current_grid_size = 8; break;
            case 1: current_grid_size = 16; break;
            case 2: current_grid_size = 32; break;
            case 3: current_grid_size = 128; break;
            case 4: current_grid_size = 0; break;
         }

         if(current_grid_size)
            create_grid(current_grid_size);
         else
            grid = NULL;
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
   else
   {
      field_click_func(x, y, state, mouse);
   }
}

object_t* create_editor_drawer(void)
{
   editor_black_texture = rm_getn(TEXTURE, "__generated_mt_grass_trans_0.0_0.0_0.0");

   run_button_pos = vec2f(895, 698);
   toolbar_eraser_pos = vec2f(16 ,704);
   toolbar_grid_pos = vec2f(16 ,752);
   toolbar_wall_pos = vec2f(80 ,752);
   toolbar_start_pos = vec2f(120 ,752);
   toolbar_fin_pos = vec2f(160 ,752);
   toolbar_obstacle_pos = vec2f(200 ,752);
   toolbar_slip_pos = vec2f(240 ,752);
   create_grid(32);
   grid_state = 2;
   current_grid_size = 32;

   tabbutton_ffnn = rm_getn(TEXTURE, "editor_tabbutton_ffnn");
   tabbutton_ga = rm_getn(TEXTURE, "editor_tabbutton_ga");
   tabbutton_map = rm_getn(TEXTURE, "editor_tabbutton_map");
   tabbutton_file = rm_getn(TEXTURE, "editor_tabbutton_file");

   toolbar_selected = rm_getn(TEXTURE, "editor_toolbar_selected");
   toolbar_frame = rm_getn(TEXTURE, "editor_toolbar_frame");
   toolbar_back = rm_getn(TEXTURE, "editor_toolbar_frame_back");
   toolbar_back = rm_getn(TEXTURE, "editor_toolbar_frame_back");
   toolbar_back_selected = rm_getn(TEXTURE, "editor_toolbar_frame_back_selected");
   toolbar_clicked = rm_getn(TEXTURE, "editor_toolbar_frame_clicked");

   run_button_texture[0] = rm_getn(TEXTURE, "editor_run");
   run_button_texture[1] = rm_getn(TEXTURE, "editor_run_selected");

   toolbar_grid_texture[0] = rm_getn(TEXTURE, "editor_toolbar_grid_8");
   toolbar_grid_texture[1] = rm_getn(TEXTURE, "editor_toolbar_grid_16");
   toolbar_grid_texture[2] = rm_getn(TEXTURE, "editor_toolbar_grid_32");
   toolbar_grid_texture[3] = rm_getn(TEXTURE, "editor_toolbar_grid_128");
   toolbar_grid_texture[4] = rm_getn(TEXTURE, "editor_toolbar_grid_no");

   toolbar_eraser_texture = rm_getn(TEXTURE, "editor_toolbar_eraser");
   toolbar_wall_texture = rm_getn(TEXTURE, "editor_toolbar_wall");
   toolbar_start_texture = rm_getn(TEXTURE, "editor_toolbar_start");
   toolbar_fin_texture = rm_getn(TEXTURE, "editor_toolbar_finish");
   toolbar_obstacle_texture = rm_getn(TEXTURE, "editor_toolbar_obstacle");
   toolbar_slip_texture = rm_getn(TEXTURE, "editor_toolbar_slip");

   toolbar_state = WALL;
   selected_toolbar_state_pos = toolbar_wall_pos;

   object_t* this = o_create();
   this->update_func = update_editor;
   this->mouse_event_func = mouse_editor;
   return this;
}


