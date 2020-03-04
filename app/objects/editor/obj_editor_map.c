//
// Created by maxim on 2/20/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_map.h"
#endif
#include "obj_editor_map.h"
#include "map_saver.h"
#include "obj_editor_drawer.h"
#include "../../rendering/renderer.h"
#include "../../../lib/resources/rmanager.h"

texture_t* texture_start;
texture_t* texture_fin;

list_t* walls = NULL;
list_t* map_objects = NULL;
float rotation;

vec2 prev_point;
bool first_point_set = false;

void draw_line(vec2 p1, vec2 p2)
{
   gr_pq_push_line(2, p1, p2, 4, COLOR_WHITE, default_primitive_renderer, NULL);
}

vec2 floor_point(vec2 pos)
{
   pos.x = current_grid_size != 0 ? roundf((float)pos.x / (float) current_grid_size) * (float) current_grid_size : pos.x;
   pos.y = current_grid_size != 0 ? roundf((float)pos.y / (float) current_grid_size) * (float) current_grid_size : pos.y;
   return pos;
}

void wheel_mouse_editor_map(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if(state == MOUSE_RELEASE && mouse == MOUSE_WHEELDOWN)
      rotation -= M_PI / (current_grid_size == 0 ? 72 : 36);
   else if(state == MOUSE_RELEASE && mouse == MOUSE_WHEELUP)
      rotation += M_PI / (current_grid_size == 0 ? 72 : 36);
}

bool get_intersection(double ray_x1, double ray_y1, double ray_x2, double ray_y2, vec2 seg1, vec2  seg2,
                      double* rx, double* ry, double* dist)
{
   double r_px = ray_x1;
   double r_py = ray_y1;
   double r_dx = ray_x2 - ray_x1;
   double r_dy = ray_y2 - ray_y1;

   double s_px = seg1.x;
   double s_py = seg1.y;
   double s_dx = seg2.x - seg1.x;
   double s_dy = seg2.y - seg1.y;

   double r_mag = sqrt(r_dx * r_dx + r_dy * r_dy);
   double s_mag = sqrt(s_dx * s_dx + s_dy * s_dy);

   if (fabs(r_dx / r_mag - s_dx / s_mag) < GET_INTERSECTION_COMP &&
       fabs(r_dy / r_mag - s_dy / s_mag) < GET_INTERSECTION_COMP)
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

void calculate_start_points(vec2 pos, float angle, vec2* p1, vec2* p2)
{
   const double max_dist = sqrtf(2) * (float)default_win->w;

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
      bool found = get_intersection(pos.x, pos.y, p1->x, p1->y, wall->p1, wall->p2, &dest_x, &dest_y, &dest_dist);
      if(found && min_p1 > dest_dist)
      {
         min_p1_x = dest_x;
         min_p1_y = dest_y;
         min_p1 = dest_dist;
         found_p1 = true;
      }

      found = get_intersection(pos.x, pos.y, p2->x, p2->y, wall->p1, wall->p2, &dest_x, &dest_y, &dest_dist);
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

void draw_line_arrow(vec2 p1, vec2 p2, float l, float w, float h, vec4 color)
{
   //calculate direction
   vec2 d = vec2_normalize(vec2f(p2.x - p1.x, p2.y - p1.y));

   //calculate normal
   vec2 n = vec2_normal(d);

   vec2 src = vec2f((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
   vec2 dest = vec2f(src.x + n.x * l, src.y + n.y * l);

   vec2 a = vec2f(dest.x - n.x * h + d.x * w, dest.y - n.y * h + d.y * w);
   vec2 b = vec2f(dest.x - n.x * h - d.x * w, dest.y - n.y * h - d.y * w);

   gr_pq_push_line(1, src, dest, 4, color, default_primitive_renderer, NULL);
   gr_pq_push_line(1, dest, a, 4, color, default_primitive_renderer, NULL);
   gr_pq_push_line(1, dest, b, 4, color, default_primitive_renderer, NULL);
}

void update_editor_map(object_t* this)
{
   static float trans = 0;
   for(size_t i = 0; i < map_objects->count; i++)
   {
      struct _map_object* object = map_objects->collection[i];

      switch(object->type)
      {
         case START:
         case FIN:
            gr_pq_push_line(1, object->p1, object->p2, 4, COLOR_BLACK, default_primitive_renderer, NULL);
            gr_pq_push_sprite(1, object->type == START ? texture_start : texture_fin,
                  vec2f(object->p1.x, object->p1.y - texture_start->height),
                  vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
            gr_pq_push_sprite(1, object->type == START ? texture_start : texture_fin,
                  vec2f(object->p2.x, object->p2.y - texture_start->height),
                  vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
            if(object->type == START)
               draw_line_arrow(object->p1, object->p2, 40, 4, 10, COLOR_GREEN);
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
      vec2 pos = floor_point(u_get_mouse_pos());
      draw_line(prev_point, pos);
   }

   if(toolbar_state == START || toolbar_state == FIN)
   {
      vec2 pos = floor_point(u_get_mouse_pos());
      float angle = current_grid_size == 0 ? rotation : (float)(roundf(rotation / (M_PI / 36)) * M_PI / 36);
      vec2 p1, p2;
      calculate_start_points(pos, angle, &p1, &p2);

      vec2 pp1 = vec2f(
            pos.x - 50 * cosf(angle),
            pos.y - 50 * sinf(angle));
      vec2 pp2 = vec2f(
            pos.x - 50 * cosf(angle+ M_PI),
            pos.y - 50 * sinf(angle+ M_PI));

      gr_pq_push_line(1, pp1, pp2, 4, COLOR_RED, default_primitive_renderer, NULL);

      gr_pq_push_line(1, p1, p2, 4, COLOR_BLACK, default_primitive_renderer, NULL);

      gr_pq_push_sprite(1, toolbar_state == START ? texture_start : texture_fin, vec2f(p1.x, p1.y - texture_start->height),
            vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
      gr_pq_push_sprite(1, toolbar_state == START ? texture_start : texture_fin, vec2f(p2.x, p2.y - texture_start->height),
            vec2f(1, 1), vec2f(0, 0), 0, default_sprite_renderer, &trans);
      if(toolbar_state == START)
         draw_line_arrow(p1, p2, 40, 4, 10, COLOR_GREEN);
   }
}

void mouse_editor_map(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   vec2 pos = floor_point(vec2f(x, y));
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
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && (toolbar_state == START || toolbar_state == FIN))
   {
      struct _map_object* object = malloc(sizeof(struct _map_object));
      object->rotation = rotation;
      object->type = toolbar_state;
      object->pos = pos;

      vec2 pos = floor_point(u_get_mouse_pos());
      float angle = current_grid_size == 0 ? rotation : (float)(roundf(rotation / (M_PI / 36)) * M_PI / 36);
      vec2 p1, p2;
      calculate_start_points(pos, angle, &p1, &p2);

      object->p1 = p1;
      object->p2 = p2;

      list_push(map_objects, object);
   }
}


object_t* create_editor_map(void)
{
   if(!walls)
      walls = list_create();
   if(!map_objects)
      map_objects = list_create();

   texture_start = rm_getn(TEXTURE, "editor_start_flag");
   texture_fin = rm_getn(TEXTURE, "editor_finish_flag");

   object_t* this = o_create();
   this->update_func = update_editor_map;
   this->mouse_event_func = wheel_mouse_editor_map;
   field_click_func = mouse_editor_map;

   map_load(walls, map_objects, MAP_SAVE_DIR "/1.map", &prev_point, &first_point_set);

   return this;
}
