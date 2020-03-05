//
// Created by maxim on 2/27/20.
//

#ifdef __GNUC__
#pragma implementation "map_builder.h"
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#include "map_builder.h"
#include "../editor/map_saver.h"
#include "../../../lib/resources/model_push.h"
#include "../../win_defaults.h"
#include "../editor/obj_editor_map.h"

struct _normal
{
   vec2 p1, p2, p;
   vec2 d, n;
};

static bool cmp_points(vec2 p1, vec2 p2, float dist)
{
   return vec2_dist(p1, p2) < dist;
}

static bool cmp_vectors(vec4 p1, vec4 p2, float dist)
{
   return vec4_dist(p1, p2) < dist;
}

static size_t find_vector(model_t* m, size_t* counter, bool normal, vec4 to_find, float min_dist)
{
   vec4* array = normal ? m->normals : m->tex_coords;
   size_t len = normal ? m->model_len->normals_count : m->model_len->tex_coords_count;

   int64_t index = -1;
   for(size_t i = 0; i < len; i++)
   {
      if(cmp_vectors(array[i], to_find, min_dist))
      {
         //vec4_free(to_find);
         index = i + 1;
         break;
      }
   }

   if(index == -1)
   {
      index = (*counter)++;
      if(normal)
         m_push_normal(m, to_find);
      else
         m_push_tex_coord(m, to_find);
   }

   return (size_t)index;
}

static model_face_t* alloc_face(
      model_t* m,
      size_t* normal_counter, size_t* tex_counter,
      vec4 t1, vec4 t2, vec4 t3, vec4 t4,
      size_t p1id, size_t p2id, size_t p3id, size_t p4id,
      vec4 normal)
{
   model_face_t* f = DEEPCARS_MALLOC(sizeof(model_face_t));
   f->count = 4;

   f->vert_id[0] = p1id;
   f->vert_id[1] = p2id;
   f->vert_id[2] = p3id;
   f->vert_id[3] = p4id;

   size_t normal_index = find_vector(m, normal_counter, true, normal, MIN_DIST_FIND_NORMAL);

   size_t t1_index = find_vector(m, tex_counter, false, vec4_ccpy(t1), MIN_DIST_FIND_TEXCOORD);
   size_t t2_index = find_vector(m, tex_counter, false, vec4_ccpy(t2), MIN_DIST_FIND_TEXCOORD);
   size_t t3_index = find_vector(m, tex_counter, false, vec4_ccpy(t3), MIN_DIST_FIND_TEXCOORD);
   size_t t4_index = find_vector(m, tex_counter, false, vec4_ccpy(t4), MIN_DIST_FIND_TEXCOORD);

   f->normal_id[0] = normal_index;
   f->normal_id[1] = normal_index;
   f->normal_id[2] = normal_index;
   f->normal_id[3] = normal_index;

   f->tex_id[0] = t1_index;
   f->tex_id[1] = t2_index;
   f->tex_id[2] = t3_index;
   f->tex_id[3] = t4_index;

   return f;
}

static struct _normal create_normal(vec2 p1, vec2 p2)
{
   struct _normal n;
   n.d = vec2fp(p1, p2);
   n.n = vec2_normal(n.d);
   n.p1 = p1;
   n.p2 = p2;
   n.p = vec2f((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
   return n;
}

static bool check_intersection(double ray_x1, double ray_y1, double ray_x2, double ray_y2, vec2 seg1, vec2 seg2)
{
   double r_px = ray_x1;
   double r_py = ray_y1;
   double r_dx = ray_x2 - ray_x1;
   double r_dy = ray_y2 - ray_y1;

   double s_px = seg1.x;
   double s_py = seg1.y;
   double s_dx = seg2.x - seg1.x;
   double s_dy = seg2.y - seg1.y;

   if ((fabs(r_dx)) < GET_INTERSECTION_COMP) // Vertical vector
   {
      double min_sx = fmin(seg1.x, seg2.x);
      double min_sy = fmin(seg1.y, seg2.y);
      double min_ry = fmin(ray_y1, ray_y2);
      double max_sx = fmax(seg1.x, seg2.x);
      double max_sy = fmax(seg1.y, seg2.y);
      double max_ry = fmax(ray_y1, ray_y2);

      if (fabs(s_dx) < GET_INTERSECTION_COMP) return false; // Parallel
      else if(fabs(s_dy) < GET_INTERSECTION_COMP)
         return max_sx > r_px && r_px > min_sx && max_ry > s_py && s_py > min_ry; // Normal
      else // Other
      {
         vec2 norm = vec2_normalize(vec2f(s_dx, s_dy));
         double y = (min_sx - norm.x * r_px) / norm.y;
         return (max_sy > y && y > min_sy && max_ry > y && y > min_ry);
      }
   }

   if ((fabs(r_dy)) < GET_INTERSECTION_COMP) // Horizontal vector
   {
      double min_sx = fmin(seg1.x, seg2.x);
      double min_sy = fmin(seg1.y, seg2.y);
      double min_rx = fmin(ray_x1, ray_x2);
      double max_sx = fmax(seg1.x, seg2.x);
      double max_sy = fmax(seg1.y, seg2.y);
      double max_rx = fmax(ray_x1, ray_x2);

      if (fabs(s_dy) < GET_INTERSECTION_COMP) return false; // Parallel
      else if(fabs(s_dx) < GET_INTERSECTION_COMP)
         return max_sy > r_py && r_py > min_sy && max_rx > s_px && s_px > min_rx; // Normal
      else // Other
      {
         vec2 norm = vec2_normalize(vec2f(s_dx, s_dy));
         double x = (min_sy - norm.y * r_py) / norm.x;
         return (max_sx > x && x > min_sx && max_rx > x && x > min_rx);
      }
   }

   return get_intersection(ray_x1, ray_y1, ray_x2, ray_y2, seg1, seg2, &r_px, &r_py, &r_dx);
}

static bool intersects(struct _normal n, struct _normal n1, struct _normal n2, struct _normal n3)
{
   vec2 dest = vec2f(n.p.x + n.n.x * 10000, n.p.y + n.n.y * 10000);

   if(check_intersection(n.p.x, n.p.y, dest.x, dest.y, n1.p1, n1.p2))
      return true;
   if(check_intersection(n.p.x, n.p.y, dest.x, dest.y, n2.p1, n2.p2))
      return true;
   if(check_intersection(n.p.x, n.p.y, dest.x, dest.y, n3.p1, n3.p2))
      return true;

   return false;
}

static void reverse(struct _normal* n, size_t i, struct _normal normals[4])
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

static void push_height_rec(
      model_t* m, size_t* i, size_t* normal_counter, size_t* tex_counter,
      vec2 p1, vec2 p2, vec2 p3, vec2 p4,
      float height,
      bool default_tex, float reduce_v, vec2 reduce_d,
      bool last, bool first)
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

   vec4 dt1 = cvec4(0, 1, 0, 0);
   vec4 dt2 = cvec4(1, 1, 0, 0);
   vec4 dt3 = cvec4(1, 0, 0, 0);
   vec4 dt4 = cvec4(0, 0, 0, 0);

   vec4 t1 = default_tex ? dt1 : cvec4(0, reduce_v * reduce_d.x + (1 - reduce_v) * reduce_d.y, 0, 0);
   vec4 t2 = default_tex ? dt2 : cvec4(1, reduce_v * reduce_d.x + (1 - reduce_v) * reduce_d.y, 0, 0);
   vec4 t3 = default_tex ? dt3 : cvec4(1, 0, 0, 0);

   vec4 tt1 = default_tex ? dt1 : cvec4(0, 1, 0, 0);
   vec4 tt2 = default_tex ? dt2 : cvec4(reduce_v * reduce_d.x + (1 - reduce_v) * reduce_d.y, 1, 0, 0);
   vec4 tt3 = default_tex ? dt3 : cvec4(reduce_v * reduce_d.x + (1 - reduce_v) * reduce_d.y, 0, 0, 0);

   /*model_face_t* face5 = alloc_face(m, normal_counter, tex_counter, t1, t2, t3, t4,
         p1id, p2id, p4id, p3id, cvec4(0, -1, 0, 0));*/
   model_face_t* face6 = alloc_face(m, normal_counter, tex_counter, t1, t2, t3, dt4,
         p1hid, p2hid, p4hid, p3hid, cvec4(0, 1, 0, 0));

   struct _normal normals[4] =
   {
      create_normal(p1, p2),
      create_normal(p3, p4),
      create_normal(p4, p2),
      create_normal(p3, p1),
   };

   for(size_t j = 0; j < 4; j++)
      reverse(&(normals[j]), j, normals);

   // Texture mapping not used here
   if(first)
   {
      model_face_t* face1 = alloc_face(m, normal_counter, tex_counter, dt1, dt2, dt3, dt4,
            p1id, p2id, p2hid, p1hid, cvec4(normals[0].n.x, 0, normals[0].n.y, 0));

      m_push_face(m, face1);
   }

   if(last)
   {
      model_face_t* face4 = alloc_face(m, normal_counter, tex_counter, dt1, dt2, dt3, dt4,
            p3id, p4id, p4hid, p3hid, cvec4(normals[1].n.x, 0, normals[1].n.y, 0));

      m_push_face(m, face4);
   }
   model_face_t* face2 = alloc_face(m, normal_counter, tex_counter, tt1, tt2, tt3, dt4,
         p4id, p2id, p2hid, p4hid, cvec4(normals[2].n.x, 0, normals[2].n.y, 0));
   model_face_t* face3 = alloc_face(m, normal_counter, tex_counter, tt1, tt2, tt3, dt4,
         p3id, p1id, p1hid, p3hid, cvec4(normals[3].n.x, 0, normals[3].n.y, 0));

   m_push_face(m, face2);
   m_push_face(m, face3);
   //m_push_face(m, face5); We dont need bottom face
   m_push_face(m, face6);
}

static void push_hexahedron(
      model_t* m, size_t* i, size_t* normal_counter, size_t* tex_counter,
      vec2 p1, vec2 p2, vec2 p3, vec2 p4,
      float height)
{
   float len = vec2_dist(p1, p3);
   size_t n = len / (WALL_WIDTH * 2);
   float rest_len = len - (float)n * (WALL_WIDTH * 2);
   float step = (WALL_WIDTH * 2) / (len);

   // Lerp p1 and p3, p2 and p4
   for(size_t s = 0; s < n; s++)
   {
      float v1 = s * step;
      float v2 = (float)(s + 1) * step;

      vec2 int_p1 = vec2_lerp(p1, p3, v1);
      vec2 int_p2 = vec2_lerp(p2, p4, v1);
      vec2 int_p3 = vec2_lerp(p1, p3, v2);
      vec2 int_p4 = vec2_lerp(p2, p4, v2);

      push_height_rec(m, i, normal_counter, tex_counter, int_p1, int_p2, int_p3, int_p4, height,
            true, 0, vec2e, i == 0, false);
   }

   float last_v = n * step;
   vec2 int_p1 = vec2_lerp(p1, p3, last_v);
   vec2 int_p2 = vec2_lerp(p2, p4, last_v);

   // Proceed last one
   vec2 d = vec2_normalize(vec2fp(p1, p3));
   float l = rest_len / (WALL_WIDTH * 2);

   push_height_rec(m, i, normal_counter, tex_counter, int_p1, int_p2, p1, p2, height, false,
         l, d, false, true);
}

static void push_prism(
      model_t* m, size_t* i, size_t* normal_counter, size_t* tex_counter,
      vec2 p1, vec2 p2, vec2 p3, vec2 p4,
      float height)
{
   push_height_rec(m, i, normal_counter, tex_counter, p1, p2, p3, p4, height,
         true, 0, vec2e, true, true);
}

model_t* build_map_model(list_t* walls)
{
   model_t* model = m_create();

   char buff[50];
   snprintf(buff, sizeof(buff), "__generated_map%i", rand());
   model->name = strdup(buff);

   size_t counter          = 1;
   size_t normal_counter   = 1;
   size_t tex_counter      = 1;
   vec2   prev_offset      = vec2e;
   vec2   prev_d           = vec2e;
   vec2   first_d          = vec2e;
   double p                = 0;
   vec2   t1               = vec2e;
   vec2   t2               = vec2e;
   vec2   t3               = vec2e;
   vec2   t4               = vec2e;
   vec2   first_p1         = vec2e;
   vec2   first_p2         = vec2e;

   for(size_t i = 0; i < walls->count; i++)
   {
      wall_t* wall = walls->collection[i];
      vec2  p1 = wall->p1;
      vec2  p2 = wall->p2;
      // Direction of current wall
      vec2 d = vec2_normalize(vec2fp(p1, p2));
      // Normal to current wall
      vec2 n = vec2_normal(d);

      // Calculate first two points of wall
      vec2 mp1 = vec2f(p1.x - n.x * WALL_WIDTH + prev_offset.x, p1.y - n.y * WALL_WIDTH + prev_offset.y);
      vec2 mp2 = vec2f(p1.x + n.x * WALL_WIDTH + prev_offset.x, p1.y + n.y * WALL_WIDTH + prev_offset.y);

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

         vec2 m_point = {0, 0},
               d1_point = {0, 0},
               d2_point = {0, 0};

         // Find matching points
         if     (cmp_points(t1, t3, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t4; }
         else if(cmp_points(t1, t4, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t3; }
         else if(cmp_points(t2, t3, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t4; }
         else if(cmp_points(t2, t4, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t3; }
         else APP_ERROR("Unable to find matching points",0);

         // Calculate fourth point of mesh
         vec2 d3_point = vec2f(d1_point.x + prev_d.x * p, d1_point.y + prev_d.y * p);
         push_prism(model, &counter, &normal_counter, &tex_counter, m_point, d1_point, d2_point, d3_point, WALL_HEIGHT);
      }

      prev_d = d;
      vec2 offset = vec2e;
      if(i != walls->count - 1)
      {
         wall_t* next_wall = walls->collection[i + 1];
         vec2 np1 = next_wall->p1;
         vec2 np2 = next_wall->p2;

         // Calculate direction of next wall
         vec2 nd = vec2_normalize(vec2fp(np1, np2));

         // Cosine of angle between current and next wall (a)
         double cos_nd_d = vec2_dot(d, nd);

         // Tan(a/2) of its angle
         double tan_half_a = sqrt((1 - cos_nd_d) / (cos_nd_d + 1));
         double l = WALL_WIDTH * tan_half_a;
         p = 2 * l;

         offset = vec2f(d.x * l, d.y * l);
         prev_offset = vec2f(nd.x * l, nd.y * l);
      }

      vec2 mp4 = vec2f(p2.x + n.x * WALL_WIDTH - offset.x, p2.y + n.y * WALL_WIDTH - offset.y);
      vec2 mp3 = vec2f(p2.x - n.x * WALL_WIDTH - offset.x, p2.y - n.y * WALL_WIDTH - offset.y);

      t1 = mp4;
      t2 = mp3;

      push_hexahedron(model, &counter, &normal_counter, &tex_counter, mp1, mp2, mp3, mp4, WALL_HEIGHT);

      // Last wall
      if(i == walls->count - 1)
      {

         t4 = first_p1;
         t3 = first_p2;

         vec2 m_point = vec2e,
              d1_point = vec2e,
              d2_point = vec2e;

         //find matching points
         if     (cmp_points(t1, t3, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t4; }
         else if(cmp_points(t1, t4, MIN_DIST)) { m_point = t1; d1_point = t2; d2_point = t3; }
         else if(cmp_points(t2, t3, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t4; }
         else if(cmp_points(t2, t4, MIN_DIST)) { m_point = t2; d1_point = t1; d2_point = t3; }
         else
         {
            break;
         }

         vec2 d3_point = vec2f(d1_point.x + first_d.x * p, d1_point.y + first_d.y * p);
         push_prism(model, &counter, &normal_counter, &tex_counter, m_point, d1_point, d2_point, d3_point, WALL_HEIGHT);
      }
   }

   return model;
}

#pragma clang diagnostic pop