//
// Created by maxim on 2/27/20.
//

#ifdef __GNUC__
#pragma implementation "map_builder.h"
#endif
#include "map_builder.h"
#include "../editor/map_saver.h"
#include "../../../lib/resources/model_push.h"

#define WALL_HEIGHT 30
#define WALL_WIDTH 10

model_face_t* alloc_face(size_t p1id, size_t p2id, size_t p3id, size_t p4id)
{
   model_face_t* f = malloc(sizeof(model_face_t));
   f->count = 4;

   f->vert_id[0] = p1id;
   f->vert_id[1] = p2id;
   f->vert_id[2] = p3id;
   f->vert_id[3] = p4id;

   f->normal_id[0] = 1;
   f->normal_id[1] = 1;
   f->normal_id[2] = 1;
   f->normal_id[3] = 1;

   f->tex_id[0] = 1;
   f->tex_id[1] = 2;
   f->tex_id[2] = 3;
   f->tex_id[3] = 4;

   return f;
}

void push_height_rec(model_t* m, size_t* i, vec2f_t p1, vec2f_t p2, vec2f_t p3, vec2f_t p4, float height)
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

   m_push_normal(m, cvec4(1, 0, 0, 0));

   model_face_t* face1 = alloc_face(p1id, p2id, p2hid, p1hid);
   model_face_t* face4 = alloc_face(p3id, p4id, p4hid, p3hid);

   model_face_t* face5 = alloc_face(p1id, p2id, p4id, p3id);
   model_face_t* face6 = alloc_face(p1hid, p2hid, p4hid, p3hid);

   model_face_t* face2 = alloc_face(p4id, p2id, p2hid, p4hid);
   model_face_t* face3 = alloc_face(p3id, p1id, p1hid, p3hid);

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
   vec2f_t prev_offset = vec2f(0, 0);

   vec2f_t t1;
   vec2f_t t2;
   vec2f_t t3;
   vec2f_t t4;

   for(size_t i = 0; i < walls->count; i++)
   {
      wall_t* wall = walls->collection[i];
      vec2f_t p1 = wall->p1;
      vec2f_t p2 = wall->p2;

      vec2f_t d = vec2f(p2.x - p1.x, p2.y - p1.y);
      vec2f_t n = vec2f(d.y, -d.x);

      float n_len = sqrtf(n.x * n.x + n.y * n.y);
      //normalize normal
      n.x /= n_len;
      n.y /= n_len;
      //normalize direction
      d.x /= n_len;
      d.y /= n_len;

      vec2f_t mp1 = vec2f(p1.x - n.x * WALL_WIDTH + prev_offset.x, p1.y - n.y * WALL_WIDTH + prev_offset.y);
      vec2f_t mp2 = vec2f(p1.x + n.x * WALL_WIDTH + prev_offset.x, p1.y + n.y * WALL_WIDTH + prev_offset.y);

      if(i != walls->count - 1 && i)
      {
         t4 = mp1;
         push_height_rec(model, &counter, t1, t2, t3, t4, WALL_HEIGHT);
      }

      vec2f_t offset = vec2f(0, 0);
      if(i != walls->count - 1)
      {
         wall_t* next_wall = walls->collection[i + 1];
         vec2f_t np1 = next_wall->p1;
         vec2f_t np2 = next_wall->p2;
         vec2f_t nd = vec2f(np2.x - np1.x, np2.y - np1.y);

         double nd_len = sqrtf(nd.x * nd.x + nd.y * nd.y);
         nd.x /= nd_len;
         nd.y /= nd_len;

         double cos_nd_d = (d.x * nd.x + d.y * nd.y);

         double nd_anlge = acos(cos_nd_d);
         double l = WALL_WIDTH * tan(nd_anlge / 2.0);
         offset = vec2f(d.x * l, d.y * l);
         prev_offset = vec2f(nd.x * l, nd.y * l);

         printf("%f\n", nd_anlge * 180 / M_PI);
         double p = 1; //2 * WALL_WIDTH / tan(nd_anlge / 2.0);

         t1 = vec2f(p2.x + n.x * WALL_WIDTH - offset.x, p2.y + n.y * WALL_WIDTH - offset.y);
         t2 = vec2f(p2.x - n.x * WALL_WIDTH - offset.x, p2.y - n.y * WALL_WIDTH - offset.y);
         t3 = vec2f(
               p2.x - n.x * WALL_WIDTH - offset.x + d.x * p,
               p2.y - n.y * WALL_WIDTH - offset.y + d.y * p);
      }

      vec2f_t mp4 = vec2f(p2.x + n.x * WALL_WIDTH - offset.x, p2.y + n.y * WALL_WIDTH - offset.y);
      vec2f_t mp3 = vec2f(p2.x - n.x * WALL_WIDTH - offset.x, p2.y - n.y * WALL_WIDTH - offset.y);
      push_height_rec(model, &counter, mp1, mp2, mp3, mp4, WALL_HEIGHT);
   }

   return model;
}


