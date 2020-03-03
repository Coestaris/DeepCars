//
// Created by maxim on 2/27/20.
//

#ifdef __GNUC__
#pragma implementation "map_builder.h"
#endif
#include "map_builder.h"
#include "../editor/map_saver.h"
#include "../../../lib/resources/model_push.h"
#include "../../win_defaults.h"
#include "../editor/obj_editor_map.h"

#define WALL_HEIGHT 10
#define WALL_WIDTH 10
#define MIN_DIST 0.1

bool cmp_points(vec2f_t p1, vec2f_t p2, float dist)
{
   return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)) < dist;
}

bool cmp_vectors(vec4 p1, vec4 p2, float dist)
{
   return vec4_dist(p1, p2) < dist;
}

model_face_t* alloc_face(model_t* m, size_t* normal_counter, size_t p1id, size_t p2id, size_t p3id, size_t p4id, vec4 normal)
{
   model_face_t* f = malloc(sizeof(model_face_t));
   f->count = 4;

   f->vert_id[0] = p1id;
   f->vert_id[1] = p2id;
   f->vert_id[2] = p3id;
   f->vert_id[3] = p4id;

   size_t normal_indx = -1;
   for(size_t i = 0; i < m->model_len->normals_count; i++)
   {
      if(cmp_vectors(m->normals[i], normal, MIN_DIST / 2))
      {
         normal_indx = i + 1;
         break;
      }
   }
   if(normal_indx == -1)
   {
      normal_indx = (*normal_counter)++;
      m_push_normal(m, normal);
   }

   f->normal_id[0] = normal_indx;
   f->normal_id[1] = normal_indx;
   f->normal_id[2] = normal_indx;
   f->normal_id[3] = normal_indx;

   f->tex_id[0] = 1;
   f->tex_id[1] = 2;
   f->tex_id[2] = 3;
   f->tex_id[3] = 4;

   return f;
}

vec2f_t normalize(vec2f_t n)
{
   float n_len = sqrtf(n.x * n.x + n.y * n.y);
   n.x /= n_len;
   n.y /= n_len;
   return n;
}

vec2f_t normal(vec2f_t p)
{
   return normalize(vec2f(-p.y, p.x));
}

double dot(vec2f_t v1, vec2f_t v2)
{
   return v1.x * v2.x + v1.y * v2.y;
}

struct _normal
{
   vec2f_t p1, p2, p;
   vec2f_t d, n;
};

struct _normal create_normal(vec2f_t p1, vec2f_t p2)
{
   struct _normal n;
   n.d = vec2f(p2.x - p1.x, p2.y - p1.y);
   n.n = normal(n.d);
   n.p1 = p1;
   n.p2 = p2;
   n.p = vec2f((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
   return n;
}

bool intersects(struct _normal n, struct _normal n1, struct _normal n2, struct _normal n3)
{
   vec2f_t dest = vec2f(n.p.x + n.n.x * 10000, n.p.y + n.n.y * 10000);
   double dist, x, y;

   if(get_intersection(n.p.x, n.p.y, dest.x, dest.y, n1.p1, n1.p2, &x, &y, &dist))
      return true;
   if(get_intersection(n.p.x, n.p.y, dest.x, dest.y, n2.p1, n2.p2, &x, &y, &dist))
      return true;
   if(get_intersection(n.p.x, n.p.y, dest.x, dest.y, n3.p1, n3.p2, &x, &y, &dist))
      return true;

   return false;
}

void reverse(struct _normal* n, size_t i, struct _normal normals[4])
{
   struct _normal others[3];
   size_t c = 0;
   for(size_t j = 0; j < 4; j++)
      if(j != i) others[c++] = normals[j];

   if(intersects(*n, others[0], others[1], others[2]))
   {
      n->n.x *= -1;
      n->n.y *= -1;
   }
}

void push_height_rec(model_t* m, size_t* i, size_t* normal_counter, vec2f_t p1, vec2f_t p2, vec2f_t p3, vec2f_t p4, float height)
{
   size_t p1id, p2id, p3id, p4id, p1hid, p2hid, p3hid, p4hid;
   m_push_vertex(m, cvec4(p1.x, 0, p1.y, 0)); p1id = (*i)++;
   m_push_vertex(m, cvec4(p2.x, 0, p2.y, 0)); p2id = (*i)++;
   m_push_vertex(m, cvec4(p3.x, 0, p3.y, 0)); p3id = (*i)++;
   m_push_vertex(m, cvec4(p4.x, 0, p4.y, 0)); p4id = (*i)++;
   m_push_vertex(m, cvec4(p1.x, height, p1.y, 0)); p1hid = (*i)++;
   m_push_vertex(m, cvec4(p2.x, height, p2.y, 0)); p2hid = (*i)++;
   m_push_vertex(m, cvec4(p3.x, height, p3.y, 0)); p3hid = (*i)++;
   m_push_vertex(m, cvec4(p4.x, height, p4.y, 0)); p4hid = (*i)++;

   model_face_t* face5 = alloc_face(m, normal_counter, p1id, p2id, p4id, p3id, cvec4(0, -1, 0, 0));
   model_face_t* face6 = alloc_face(m, normal_counter, p1hid, p2hid, p4hid, p3hid, cvec4(0, 1, 0, 0));

   struct _normal normals[4] =
   {
      create_normal(p1, p2),
      create_normal(p3, p4),
      create_normal(p4, p2),
      create_normal(p3, p1),
   };

   for(size_t j = 0; j < 4; j++)
      reverse(&(normals[j]), j, normals);

   model_face_t* face1 = alloc_face(m, normal_counter, p1id, p2id, p2hid, p1hid, cvec4(normals[0].n.x, 0, normals[0].n.y, 0));
   model_face_t* face4 = alloc_face(m, normal_counter, p3id, p4id, p4hid, p3hid, cvec4(normals[1].n.x, 0, normals[1].n.y, 0));

   model_face_t* face2 = alloc_face(m, normal_counter, p4id, p2id, p2hid, p4hid, cvec4(normals[2].n.x, 0, normals[2].n.y, 0));
   model_face_t* face3 = alloc_face(m, normal_counter, p3id, p1id, p1hid, p3hid, cvec4(normals[3].n.x, 0, normals[3].n.y, 0));

   m_push_face(m, face1);
   m_push_face(m, face2);
   m_push_face(m, face3);
   m_push_face(m, face4);
   m_push_face(m, face5);
   m_push_face(m, face6);
}

model_t* build_map_model(list_t* walls)
{
   model_t* model = m_create();

   char buff[50];
   snprintf(buff, sizeof(buff), "__generated_map%i", rand());
   model->name = strdup(buff);

   m_push_tex_coord(model, cvec4(0, 1, 0, 0));
   m_push_tex_coord(model, cvec4(1, 1, 0, 0));
   m_push_tex_coord(model, cvec4(1, 0, 0, 0));
   m_push_tex_coord(model, cvec4(0, 0, 0, 0));
   size_t counter = 1;
   size_t normal_counter = 1;

   vec2f_t prev_offset = {0, 0};
   vec2f_t prev_d = {0, 0};
   vec2f_t first_d = {0, 0};
   double p = 0;

   vec2f_t t1 = {0, 0};
   vec2f_t t2 = {0, 0};
   vec2f_t t3 = {0, 0};
   vec2f_t t4 = {0, 0};

   vec2f_t first_p1 = {0, 0};
   vec2f_t first_p2 = {0, 0};

   for(size_t i = 0; i < walls->count; i++)
   {
      wall_t* wall = walls->collection[i];
      vec2f_t p1 = wall->p1;
      vec2f_t p2 = wall->p2;
      // Direction of current wall
      vec2f_t d = normalize(vec2f(p2.x - p1.x, p2.y - p1.y));
      // Normal to current wall
      vec2f_t n = normalize(vec2f(d.y, -d.x));

      // Calculate first two points of wall
      vec2f_t mp1 = vec2f(p1.x - n.x * WALL_WIDTH + prev_offset.x, p1.y - n.y * WALL_WIDTH + prev_offset.y);
      vec2f_t mp2 = vec2f(p1.x + n.x * WALL_WIDTH + prev_offset.x, p1.y + n.y * WALL_WIDTH + prev_offset.y);

      // If its first point store its values
      if(!i)
      {
         first_p1 = mp1;
         first_p2 = mp2;
         first_d = d;
      }
      else
      {
         t4 = mp1;
         t3 = mp2;

         vec2f_t m_point = {0, 0},
               d1_point = {0, 0},
               d2_point = {0, 0};

         // Find matching points
         if     (cmp_points(t1, t3, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t4; }
         else if(cmp_points(t1, t4, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t3; }
         else if(cmp_points(t2, t3, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t4; }
         else if(cmp_points(t2, t4, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t3; }
         else APP_ERROR("Unable to find matching points",0);

         // Calculate fourth point of mesh
         vec2f_t d3_point = vec2f(d1_point.x + prev_d.x * p, d1_point.y + prev_d.y * p);
         push_height_rec(model, &counter, &normal_counter, m_point, d1_point, d2_point, d3_point, WALL_HEIGHT);
      }

      prev_d = d;
      vec2f_t offset = vec2f(0, 0);
      if(i != walls->count - 1)
      {
         wall_t* next_wall = walls->collection[i + 1];
         vec2f_t np1 = next_wall->p1;
         vec2f_t np2 = next_wall->p2;
         // Calculate direction of next wall
         vec2f_t nd = normalize(vec2f(np2.x - np1.x, np2.y - np1.y));

         // Cosine of angle between current and next wall (a)
         double cos_nd_d = dot(d, nd);
         // Tan(a/2) of its angle
         double tan_half_a = sqrt((1 - cos_nd_d) / (cos_nd_d + 1));
         double l = WALL_WIDTH * tan_half_a;
         p = 2 * l;

         offset = vec2f(d.x * l, d.y * l);
         prev_offset = vec2f(nd.x * l, nd.y * l);
      }

      vec2f_t mp4 = vec2f(p2.x + n.x * WALL_WIDTH - offset.x, p2.y + n.y * WALL_WIDTH - offset.y);
      vec2f_t mp3 = vec2f(p2.x - n.x * WALL_WIDTH - offset.x, p2.y - n.y * WALL_WIDTH - offset.y);

      t1 = mp4;
      t2 = mp3;

      push_height_rec(model, &counter, &normal_counter, mp1, mp2, mp3, mp4, WALL_HEIGHT);

      // Last wall
      if(i == walls->count - 1)
      {

         t4 = first_p1;
         t3 = first_p2;

         vec2f_t m_point = {0, 0},
               d1_point = {0, 0},
               d2_point = {0, 0};

         //find matching points
         if     (cmp_points(t1, t3, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t4; }
         else if(cmp_points(t1, t4, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t3; }
         else if(cmp_points(t2, t3, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t4; }
         else if(cmp_points(t2, t4, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t3; }
         else
         {
            break;
         }

         vec2f_t d3_point = vec2f(d1_point.x + first_d.x * p, d1_point.y + first_d.y * p);
         push_height_rec(model, &counter, &normal_counter, m_point, d1_point, d2_point, d3_point, WALL_HEIGHT);
      }
   }

   return model;
}


