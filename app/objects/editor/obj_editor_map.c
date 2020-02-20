//
// Created by maxim on 2/20/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_map.h"
#endif
#include "obj_editor_map.h"
#include "obj_editor_drawer.h"
#include "../../rendering/renderer.h"
#include "../../../lib/resources/rmanager.h"

struct _wall {
   vec2f_t p1;
   vec2f_t p2;
};

struct _map_object {
   vec2f_t pos;

   vec2f_t p1;
   vec2f_t p2;

   float rotation;
   enum _toolbar_state type;
};

texture_t* texture_start;

list_t* walls;
list_t* map_objects;
float rotation;

vec2f_t prev_point;
bool first_point_set = false;

void draw_line(vec2f_t p1, vec2f_t p2)
{
   gr_pq_push_line(2, p1, p2, 4, COLOR_WHITE, default_primitive_renderer, NULL);
}

vec2f_t floor_point(vec2f_t pos)
{
   pos.x = current_grid_size != 0 ? roundf((float)pos.x / (float) current_grid_size) * (float) current_grid_size : pos.x;
   pos.y = current_grid_size != 0 ? roundf((float)pos.y / (float) current_grid_size) * (float) current_grid_size : pos.y;
   return pos;
}

void wheel_mouse_editor_map(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if(state == MOUSE_RELEASE && mouse == MOUSE_WHEELDOWN)
      rotation -= M_PI / 36;
   else if(state == MOUSE_RELEASE && mouse == MOUSE_WHEELUP)
      rotation += M_PI / 36;
}

// taken from https://github.com/Coestaris/Zomboid2.0/blob/master/client/lib/ltracer/ltracer_math.c
int getIntersection(double rayX1, double rayY1, double rayX2, double rayY2, vec2f_t seg1, vec2f_t  seg2,
                            double* rx, double* ry, double* dist)
{
   double r_px = rayX1;
   double r_py = rayY1;
   double r_dx = rayX2 - rayX1;
   double r_dy = rayY2 - rayY1;

   double s_px = seg1.x;
   double s_py = seg1.y;
   double s_dx = seg2.x - seg1.x;
   double s_dy = seg2.y - seg1.y;

   double r_mag = sqrt(r_dx * r_dx + r_dy * r_dy);
   double s_mag = sqrt(s_dx * s_dx + s_dy * s_dy);

   if (r_dx / r_mag == s_dx / s_mag && r_dy / r_mag == s_dy / s_mag)
   {
      return false;
   }

   double T2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
   double T1 = (s_px + s_dx * T2 - r_px) / r_dx;

   if (T1 < 0) return false;
   if (T2 < 0 || T2 > 1) return false;

   *rx = r_px + r_dx * T1;
   *ry = r_py + r_dy * T1;
   *dist = T1;

   return true;
}

void calculate_start_points(vec2f_t pos, float angle, vec2f_t* p1, vec2f_t* p2)
{
   const double max_dist = sqrtf(2) * default_win->w;

   *p1 = vec2f(
         pos.x - max_dist * cosf(angle),
         pos.y - max_dist * sinf(angle));
   *p2 = vec2f(
         pos.x - max_dist * cosf(angle + M_PI),
         pos.y - max_dist * sinf(angle + M_PI));

   double min_p1 = max_dist;
   double min_p2 = max_dist;
   bool found_p1 = false, found_p2 = false;

   double min_p1_x, min_p1_y;
   double min_p2_x, min_p2_y;
   double dest_x, dest_y;
   double dest_dist;

   for(size_t i = 0; i < walls->count; i++)
   {
      struct _wall* wall = walls->collection[i];
      bool found = getIntersection(pos.x, pos.y, p1->x, p1->y, wall->p1, wall->p2, &dest_x, &dest_y, &dest_dist);
      if(found && min_p1 > dest_dist)
      {
         min_p1_x = dest_x;
         min_p1_y = dest_y;
         min_p1 = dest_dist;
         found_p1 = true;
      }

      found = getIntersection(pos.x, pos.y, p2->x, p2->y, wall->p1, wall->p2, &dest_x, &dest_y, &dest_dist);
      if(found && min_p2 > dest_dist)
      {
         min_p2_x = dest_x;
         min_p2_y = dest_y;
         min_p2 = dest_dist;
         found_p2 = true;
      }
   }

   if(found_p1)
   {
      *p1 = vec2f(min_p1_x, min_p1_y);
   }
   else
   {
      *p1 = vec2f(
            pos.x - 50 * cosf(angle),
            pos.y - 50 * sinf(angle));
   }
   if(found_p2)
   {
      *p2 = vec2f(min_p2_x, min_p2_y);
   }
   else
   {
      *p2 = vec2f(
            pos.x - 50 * cosf(angle + M_PI),
            pos.y - 50 * sinf(angle + M_PI));
   }
}

void update_editor_map(object_t* this)
{
   float trans = 0;
   for(size_t i = 0; i < map_objects->count; i++)
   {
      struct _map_object* object = map_objects->collection[i];

      switch(object->type)
      {
         case START:
            gr_pq_push_line(1, object->p1, object->p2, 4, COLOR_BLACK, default_primitive_renderer, NULL);
            gr_pq_push_sprite(1, texture_start, vec2f(object->p1.x, object->p1.y - texture_start->height),
                              vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
            gr_pq_push_sprite(1, texture_start, vec2f(object->p2.x, object->p2.y - texture_start->height),
                              vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
            break;
         case ERASER:
         case OBSTACLE:
         case SLIP:
         case WALL:
            break;
      }
   }

   for(size_t i = 0; i < walls->count; i++)
   {
      struct _wall* wall = walls->collection[i];
      draw_line(wall->p1, wall->p2);
   }

   if(toolbar_state == WALL && (walls->count != 0 || first_point_set))
   {
      vec2f_t pos = floor_point(u_get_mouse_pos());
      draw_line(prev_point, pos);
   }

   if(toolbar_state == START)
   {
      vec2f_t pos = floor_point(u_get_mouse_pos());
      float angle = roundf(rotation / (M_PI / 36)) * M_PI / 36;
      vec2f_t p1, p2;
      calculate_start_points(pos, angle, &p1, &p2);

      vec2f_t pp1 = vec2f(
            pos.x - 50 * cosf(angle),
            pos.y - 50 * sinf(angle));
      vec2f_t pp2 = vec2f(
            pos.x - 50 * cosf(angle+ M_PI),
            pos.y - 50 * sinf(angle+ M_PI));

      gr_pq_push_line(1, pp1, pp2, 4, COLOR_RED, default_primitive_renderer, NULL);

      gr_pq_push_line(1, p1, p2, 4, COLOR_BLACK, default_primitive_renderer, NULL);

      gr_pq_push_sprite(1, texture_start, vec2f(p1.x, p1.y - texture_start->height),
            vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
      gr_pq_push_sprite(1, texture_start, vec2f(p2.x, p2.y - texture_start->height),
            vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
   }
}

void mouse_editor_map(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   vec2f_t pos = floor_point(vec2f(x, y));
   if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && toolbar_state == WALL)
   {
      if(first_point_set)
      {
         struct _wall* wall = malloc(sizeof(struct _wall));
         wall->p1 = prev_point;
         wall->p2 = pos;
         list_push(walls, wall);
      }
      prev_point = pos;
      first_point_set = true;
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && toolbar_state == START)
   {
      struct _map_object* object = malloc(sizeof(struct _map_object));
      object->rotation = rotation;
      object->type = START;
      object->pos = pos;

      vec2f_t pos = floor_point(u_get_mouse_pos());
      float angle = roundf(rotation / (M_PI / 36)) * M_PI / 36;
      vec2f_t p1, p2;
      calculate_start_points(pos, angle, &p1, &p2);

      object->p1 = p1;
      object->p2 = p2;

      list_push(map_objects, object);
   }
}


object_t* create_editor_map(void)
{
   texture_start = rm_getn(TEXTURE, "editor_finish_flag");

   walls = list_create();
   map_objects = list_create();

   object_t* this = o_create();
   this->update_func = update_editor_map;
   this->mouse_event_func = wheel_mouse_editor_map;
   field_click_func = mouse_editor_map;
   return this;
}
