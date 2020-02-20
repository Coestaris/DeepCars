//
// Created by maxim on 2/18/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_drawer.h"
#endif
#include "obj_editor_drawer.h"
#include "../../../lib/resources/rmanager.h"
#include "../../rendering/renderer.h"

texture_t* run_button_texture[2];
texture_t* toolbar_eraser_texture[3];
texture_t* toolbar_grid_texture[5][3];
texture_t* toolbar_obstacle_texture[3];
texture_t* toolbar_slip_texture[3];
texture_t* toolbar_start_texture[3];
texture_t* toolbar_wall_texture[3];
texture_t* toolbar_selected;

texture_t* grid;
texture_t* editor_black_texture;

vec2f_t run_button_pos;
vec2f_t toolbar_eraser_pos;
vec2f_t toolbar_grid_pos;
vec2f_t toolbar_obstacle_pos;
vec2f_t toolbar_slip_pos;
vec2f_t toolbar_start_pos;
vec2f_t toolbar_wall_pos;

bool toolbar_eraser_clicked;
bool toolbar_grid_clicked;
bool toolbar_obstacle_clicked;
bool toolbar_slip_clicked;
bool toolbar_start_clicked;
bool toolbar_wall_clicked;

float editor_p = 0;

enum _toolbar_state toolbar_state;
vec2f_t selected_toolbar_state_pos;
size_t grid_state;
size_t current_grid_size;
void (*field_click_func)(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);

#define grid_size 256

void create_grid(size_t size)
{
   char buff[256];
   snprintf(buff, sizeof(buff), "_%li_grid", size);

   grid = rm_getn_try(TEXTURE, buff);
   if(!grid)
   {
      pngImage* image = oilCreateImg();
      image->width = grid_size;
      image->height = grid_size;
      image->colorMatrix = oilColorMatrixAlloc(true, grid_size, grid_size);

      oilColor gridColor = {65, 65, 65, 220};
      oilColor gridColor1 = {90, 90, 90, 220};
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
   gr_pq_push_sprite(4, texture, position, vec2f(1, 1),
                     vec2f(0, 0), 0, default_sprite_renderer, &a);
}

void draw_depth(size_t depth, texture_t* texture, vec2f_t position)
{
   float a = 1;
   gr_pq_push_sprite(depth, texture, position, vec2f(1, 1),
                     vec2f(0, 0), 0, default_sprite_renderer, &a);
}

#define CHECK(position, texture)                           \
   pos.x > position.x && pos.x < position.x + texture->width && \
   pos.y > position.y && pos.y < position.y + texture->height   \

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

   if(CHECK(run_button_pos, run_button_texture[0])) draw(run_button_texture[1], run_button_pos);
   else draw(run_button_texture[0], run_button_pos);

   if(CHECK(toolbar_eraser_pos, toolbar_eraser_texture[0])) draw(toolbar_eraser_texture[toolbar_eraser_clicked ? 2 : 1], toolbar_eraser_pos);
   else draw(toolbar_eraser_texture[0], toolbar_eraser_pos);

   if(CHECK(toolbar_grid_pos, toolbar_grid_texture[0][0])) draw(toolbar_grid_texture[grid_state][toolbar_grid_clicked ? 2 : 1], toolbar_grid_pos);
   else draw(toolbar_grid_texture[grid_state][0], toolbar_grid_pos);

   if(CHECK(toolbar_obstacle_pos, toolbar_obstacle_texture[0])) draw(toolbar_obstacle_texture[toolbar_obstacle_clicked ? 2 : 1], toolbar_obstacle_pos);
   else draw(toolbar_obstacle_texture[0], toolbar_obstacle_pos);

   if(CHECK(toolbar_slip_pos, toolbar_slip_texture[0])) draw(toolbar_slip_texture[toolbar_slip_clicked ? 2 : 1], toolbar_slip_pos);
   else draw(toolbar_slip_texture[0], toolbar_slip_pos);

   if(CHECK(toolbar_start_pos, toolbar_start_texture[0])) draw(toolbar_start_texture[toolbar_start_clicked ? 2 : 1], toolbar_start_pos);
   else draw(toolbar_start_texture[0], toolbar_start_pos);

   if(CHECK(toolbar_wall_pos, toolbar_wall_texture[0])) draw(toolbar_wall_texture[toolbar_wall_clicked ? 2 : 1], toolbar_wall_pos);
   else draw(toolbar_wall_texture[0], toolbar_wall_pos);

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

   if(CHECK(toolbar_eraser_pos, toolbar_eraser_texture[0]))
   {
      toolbar_eraser_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = ERASER;
         selected_toolbar_state_pos = toolbar_eraser_pos;
      }
   }
   else if(CHECK(toolbar_grid_pos, toolbar_grid_texture[grid_state][0]))
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
   else if(CHECK(toolbar_obstacle_pos, toolbar_obstacle_texture[0]))
   {
      toolbar_obstacle_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = OBSTACLE;
         selected_toolbar_state_pos = toolbar_obstacle_pos;
      }
   }
   else if(CHECK(toolbar_slip_pos, toolbar_slip_texture[0]))
   {
      toolbar_slip_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = ERASER;
         selected_toolbar_state_pos = toolbar_slip_pos;
      }
   }
   else if(CHECK(toolbar_start_pos, toolbar_start_texture[0]))
   {
      toolbar_start_clicked = state == MOUSE_PRESSED && mouse == MOUSE_LEFT;
      if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT)
      {
         toolbar_state = START;
         selected_toolbar_state_pos = toolbar_start_pos;
      }
   }
   else if(CHECK(toolbar_wall_pos, toolbar_wall_texture[0]))
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
   toolbar_obstacle_pos = vec2f(160 ,752);
   toolbar_slip_pos = vec2f(200 ,752);
   create_grid(32);
   grid_state = 2;
   current_grid_size = 32;

   toolbar_selected = rm_getn(TEXTURE, "editor_toolbar_selected");

   run_button_texture[0] = rm_getn(TEXTURE, "editor_run");
   run_button_texture[1] = rm_getn(TEXTURE, "editor_run_selected");

   toolbar_eraser_texture[0] = rm_getn(TEXTURE, "editor_toolbar_eraser");
   toolbar_eraser_texture[1] = rm_getn(TEXTURE, "editor_toolbar_eraser_selected");
   toolbar_eraser_texture[2] = rm_getn(TEXTURE, "editor_toolbar_eraser_clicked");

   toolbar_grid_texture[0][0] = rm_getn(TEXTURE, "editor_toolbar_grid8");
   toolbar_grid_texture[0][1] = rm_getn(TEXTURE, "editor_toolbar_grid8_selected");
   toolbar_grid_texture[0][2] = rm_getn(TEXTURE, "editor_toolbar_grid8_clicked");
   toolbar_grid_texture[1][0] = rm_getn(TEXTURE, "editor_toolbar_grid16");
   toolbar_grid_texture[1][1] = rm_getn(TEXTURE, "editor_toolbar_grid16_selected");
   toolbar_grid_texture[1][2] = rm_getn(TEXTURE, "editor_toolbar_grid16_clicked");
   toolbar_grid_texture[2][0] = rm_getn(TEXTURE, "editor_toolbar_grid32");
   toolbar_grid_texture[2][1] = rm_getn(TEXTURE, "editor_toolbar_grid32_selected");
   toolbar_grid_texture[2][2] = rm_getn(TEXTURE, "editor_toolbar_grid32_clicked");
   toolbar_grid_texture[3][0] = rm_getn(TEXTURE, "editor_toolbar_grid128");
   toolbar_grid_texture[3][1] = rm_getn(TEXTURE, "editor_toolbar_grid128_selected");
   toolbar_grid_texture[3][2] = rm_getn(TEXTURE, "editor_toolbar_grid128_clicked");
   toolbar_grid_texture[4][0] = rm_getn(TEXTURE, "editor_toolbar_grid_no");
   toolbar_grid_texture[4][1] = rm_getn(TEXTURE, "editor_toolbar_grid_no_selected");
   toolbar_grid_texture[4][2] = rm_getn(TEXTURE, "editor_toolbar_grid_no_clicked");

   toolbar_obstacle_texture[0] = rm_getn(TEXTURE, "editor_toolbar_obstacle");
   toolbar_obstacle_texture[1] = rm_getn(TEXTURE, "editor_toolbar_obstacle_selected");
   toolbar_obstacle_texture[2] = rm_getn(TEXTURE, "editor_toolbar_obstacle_clicked");

   toolbar_slip_texture[0] = rm_getn(TEXTURE, "editor_toolbar_slip");
   toolbar_slip_texture[1] = rm_getn(TEXTURE, "editor_toolbar_slip_selected");
   toolbar_slip_texture[2] = rm_getn(TEXTURE, "editor_toolbar_slip_clicked");

   toolbar_start_texture[0] = rm_getn(TEXTURE, "editor_toolbar_start");
   toolbar_start_texture[1] = rm_getn(TEXTURE, "editor_toolbar_start_selected");
   toolbar_start_texture[2] = rm_getn(TEXTURE, "editor_toolbar_start_clicked");

   toolbar_wall_texture[0] = rm_getn(TEXTURE, "editor_toolbar_wall");
   toolbar_wall_texture[1] = rm_getn(TEXTURE, "editor_toolbar_wall_selected");
   toolbar_wall_texture[2] = rm_getn(TEXTURE, "editor_toolbar_wall_clicked");

   toolbar_state = WALL;
   selected_toolbar_state_pos = toolbar_wall_pos;

   object_t* this = o_create();
   this->update_func = update_editor;
   this->mouse_event_func = mouse_editor;
   return this;
}


