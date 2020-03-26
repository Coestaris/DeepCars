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
#include "../../win_defaults.h"

static texture_t*  texture_marker_prev   = NULL;
static texture_t*  texture_marker_disjoint   = NULL;
static texture_t*  texture_marker   = NULL;

static texture_t*  texture_start   = NULL;
static texture_t*  texture_fin     = NULL;
static float       rotation        = 0;

list_t*     walls           = NULL;
list_t*     map_objects     = NULL;
vec2        prev_point      = vec2e;
bool        first_point_set = false;

static void draw_line(vec2 p1, vec2 p2)
{
   gr_pq_push_line(MAP_MAIN_DEPTH, p1, p2, MAP_MAIN_LINE_WIDTH, MAP_MAIN_LINE_COLOR,
         default_primitive_renderer, NULL);
}

static vec2 floor_point(vec2 pos)
{
   pos.x = current_grid_size != 0 ?
         roundf((float)pos.x / (float) current_grid_size) * (float) current_grid_size : pos.x;

   pos.y = current_grid_size != 0 ?
         roundf((float)pos.y / (float) current_grid_size) * (float) current_grid_size : pos.y;

   return pos;
}

static void wheel_mouse_editor_map(object_t* this, uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   if(state == MOUSE_RELEASE && mouse == MOUSE_WHEELDOWN)
      rotation -= (float)M_PI / (float)(current_grid_size == 0 ? MAP_ROTATION_STEP_SMALL : MAP_ROTATION_STEP);
   else if(state == MOUSE_RELEASE && mouse == MOUSE_WHEELUP)
      rotation += (float)M_PI / (float)(current_grid_size == 0 ? MAP_ROTATION_STEP_SMALL : MAP_ROTATION_STEP);
}

static void calculate_start_points(vec2 pos, float angle, vec2* p1, vec2* p2)
{
   const double max_dist = MAP_FLAGS_MAX_DIST;

   *p1 = vec2f(
         pos.x - max_dist * cosf(angle),
         pos.y - max_dist * sinf(angle));
   *p2 = vec2f(
         pos.x - max_dist * cosf(angle + M_PI),
         pos.y - max_dist * sinf(angle + M_PI));

   double min_p1    = max_dist;
   double min_p2    = max_dist;
   bool   found_p1  = false;
   bool   found_p2  = false;

   double min_p1_x  = 0;
   double min_p1_y  = 0;
   double min_p2_x  = 0;
   double min_p2_y  = 0;

   double dest_x    = 0;
   double dest_y    = 0;
   double dest_dist = 0;

   for(size_t i = 0; i < walls->count; i++)
   {
      wall_t* wall = walls->collection[i];
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
            pos.x - MAP_FLAGS_MIN_DIST * cosf(angle),
            pos.y - MAP_FLAGS_MIN_DIST * sinf(angle));
   }
   if(found_p2)
   {
      *p2 = vec2f(min_p2_x, min_p2_y);
   }
   else
   {
      *p2 = vec2f(
            pos.x - MAP_FLAGS_MIN_DIST * cosf(angle + M_PI),
            pos.y - MAP_FLAGS_MIN_DIST * sinf(angle + M_PI));
   }
}

static void draw_line_arrow(vec2 p1, vec2 p2, float l, float w, float h, vec4 color)
{
   //calculate direction
   vec2 d = vec2_normalize(vec2fp(p1, p2));

   //calculate normal
   vec2 n = vec2_normal(d);

   vec2 src = vec2f((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
   vec2 dest = vec2f(src.x + n.x * l, src.y + n.y * l);

   vec2 a = vec2f(dest.x - n.x * h + d.x * w, dest.y - n.y * h + d.y * w);
   vec2 b = vec2f(dest.x - n.x * h - d.x * w, dest.y - n.y * h - d.y * w);

   gr_pq_push_line(MAP_FLAGS_DEPTH, src, dest, MAP_FLAGS_LINE_WIDTH, color, default_primitive_renderer, NULL);
   gr_pq_push_line(MAP_FLAGS_DEPTH, dest, a, MAP_FLAGS_LINE_WIDTH, color, default_primitive_renderer, NULL);
   gr_pq_push_line(MAP_FLAGS_DEPTH, dest, b, MAP_FLAGS_LINE_WIDTH, color, default_primitive_renderer, NULL);
}

static void draw_marker(texture_t* tex, vec2 point)
{
   static float trans = 0;
   gr_pq_push_sprite(MAP_MAIN_DEPTH + 1, tex, vec2f(point.x - tex->width / 2.0, point.y - tex->height / 2.0),
                     vec2u, vec2e, 0, default_sprite_renderer, &trans);
}

static void update_editor_map(object_t* this)
{
   static float trans = 0;
   for(size_t i = 0; i < map_objects->count; i++)
   {
      struct _map_object* object = map_objects->collection[i];

      switch(object->type)
      {
         case START:
         case FIN:
            gr_pq_push_line(MAP_FLAGS_DEPTH, object->p1, object->p2, MAP_FLAGS_LINE_WIDTH, MAP_FLAGS_LINE_COLOR,
                  default_primitive_renderer, NULL);

            gr_pq_push_sprite(MAP_FLAGS_DEPTH, object->type == START ? texture_start : texture_fin,
                  vec2f(object->p1.x, object->p1.y - texture_start->height),
                  vec2u, vec2e, 0, default_sprite_renderer, &trans);

            gr_pq_push_sprite(MAP_FLAGS_DEPTH, object->type == START ? texture_start : texture_fin,
                  vec2f(object->p2.x, object->p2.y - texture_start->height),
                  vec2u, vec2e, 0, default_sprite_renderer, &trans);

            if(object->type == START)
               draw_line_arrow(object->p1, object->p2, MAP_FLAGS_ARROW_L, MAP_FLAGS_ARROW_W, MAP_FLAGS_ARROW_H,
                     MAP_FLAGS_ARROW_COLOR);
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
      wall_t* wall = walls->collection[i];
      wall_t* next_wall = i != walls->count - 1 ?  walls->collection[i + 1] : NULL;

      draw_line(wall->p1, wall->p2);

      if(!wall->looped && ((next_wall && next_wall->disjoint) || (i == walls->count - 1 && first_point_set)))
         draw_marker(texture_marker_disjoint, wall->p2);

      if(wall->disjoint && !wall->looped)
         draw_marker(texture_marker_disjoint, wall->p1);
      else
         draw_marker(texture_marker, wall->p1);
   }

   if(toolbar_state == WALL && (walls->count != 0 || first_point_set))
   {
      if(u_get_key_state(50) == KEY_PRESSED || u_get_key_state(62) == KEY_PRESSED)
      {
         vec2 pos = floor_point(u_get_mouse_pos());
         draw_line(prev_point, pos);
      }

      draw_marker(texture_marker_prev, prev_point);
   }

   if(toolbar_state == START || toolbar_state == FIN)
   {
      vec2 pos = floor_point(u_get_mouse_pos());
      float angle = current_grid_size == 0 ? rotation :
            (float)(roundf(rotation / (float)(M_PI / MAP_ROTATION_STEP)) * M_PI / MAP_ROTATION_STEP);

      vec2 p1, p2;
      calculate_start_points(pos, angle, &p1, &p2);

      vec2 pp1 = vec2f(
            pos.x - MAP_FLAGS_MIN_DIST * cosf(angle),
            pos.y - MAP_FLAGS_MIN_DIST * sinf(angle));
      vec2 pp2 = vec2f(
            pos.x - MAP_FLAGS_MIN_DIST * cosf(angle+ M_PI),
            pos.y - MAP_FLAGS_MIN_DIST * sinf(angle+ M_PI));

      gr_pq_push_line(MAP_FLAGS_DEPTH, p1, p2, MAP_FLAGS_LINE_WIDTH, MAP_FLAGS_LINE_COLOR,
            default_primitive_renderer, NULL);

      gr_pq_push_sprite(MAP_FLAGS_DEPTH, toolbar_state == START ? texture_start : texture_fin,
            vec2f(p1.x, p1.y - texture_start->height),
            vec2u, vec2e, 0, default_sprite_renderer, &trans);

      gr_pq_push_sprite(MAP_FLAGS_DEPTH, toolbar_state == START ? texture_start : texture_fin,
            vec2f(p2.x, p2.y - texture_start->height),
            vec2u, vec2e, 0, default_sprite_renderer, &trans);

      if(toolbar_state == START)
         draw_line_arrow(p1, p2, MAP_FLAGS_ARROW_L, MAP_FLAGS_ARROW_W, MAP_FLAGS_ARROW_H, MAP_FLAGS_ARROW_COLOR);
   }
}

static size_t counter = 0;

static void mouse_editor_map(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse)
{
   vec2 pos = floor_point(vec2f(x, y));
   if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && toolbar_state == WALL)
   {
      if(u_get_key_state(50) != KEY_PRESSED && u_get_key_state(62) != KEY_PRESSED)
      {
         first_point_set = false;
         counter = 0;
      }

      if(first_point_set)
      {
         if(vec2_dist(pos, prev_point) < 1)
            return;

         wall_t* wall = DEEPCARS_MALLOC(sizeof(wall_t));
         wall->looped = false;
         wall->disjoint = true;
         wall->p1 = prev_point;

         if(counter > 0)
            wall->disjoint = false;

         counter++;

         wall->p2 = pos;

         wall_t* first_of_loop = NULL;

         if(counter > 1)
         {
            size_t index = walls->count - 1;
            while (index >= 0)
            {
               wall_t* w = (wall_t*) walls->collection[index];
               if (w->disjoint)
               {
                  first_of_loop = w;
                  break;
               }
               index--;
            }
         }

         if(first_of_loop != NULL)
         {
            if(vec2_dist(first_of_loop->p1, wall->p2) < 1e-2)
            {
               wall->p2 = first_of_loop->p1;

               wall->looped = true;
               first_of_loop->looped = true;
            }
         }

         list_push(walls, wall);
      }
      prev_point = pos;
      first_point_set = true;
   }
   else if(state == MOUSE_RELEASE && mouse == MOUSE_LEFT && (toolbar_state == START || toolbar_state == FIN))
   {
      struct _map_object* object = DEEPCARS_MALLOC(sizeof(struct _map_object));
      object->rotation = rotation;
      object->type = toolbar_state;
      object->pos = pos;

      vec2 pos = floor_point(u_get_mouse_pos());
      float angle = current_grid_size == 0 ? rotation :
            (float)(roundf(rotation / (float)(M_PI / MAP_ROTATION_STEP_SMALL)) * M_PI / MAP_ROTATION_STEP_SMALL);

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

   texture_marker_prev = rm_getn(TEXTURE, "editor_marker_prev");
   texture_marker_disjoint = rm_getn(TEXTURE, "editor_marker_disjoint");
   texture_marker = rm_getn(TEXTURE, "editor_marker_default");

   texture_start = rm_getn(TEXTURE, "editor_start_flag");
   texture_fin = rm_getn(TEXTURE, "editor_finish_flag");

   object_t* this = o_create();
   this->update_func = update_editor_map;
   this->mouse_event_func = wheel_mouse_editor_map;
   field_click_func = mouse_editor_map;

#ifdef MAP_DEFAULT_MAP
   map_load(walls, map_objects, MAP_SAVE_DIR MAP_DEFAULT_MAP, &prev_point, &first_point_set);
#endif

   return this;
}
