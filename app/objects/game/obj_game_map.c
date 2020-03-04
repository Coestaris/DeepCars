//
// Created by maxim on 2/28/20.
//

#ifdef __GNUC__
#pragma implementation "obj_game_map.h"
#endif
#include "obj_game_map.h"
#include "../obj_dummy.h"
#include "map_builder.h"
#include "../editor/obj_editor_map.h"

bool first_map_load = true;

void game_map_update(object_t* this)
{
   if(first_map_load)
   {
      model_t* m = build_map_model(walls);
      m_build(m);
      //m_normalize(m, true, true, false, false);
      rm_push(MODEL, m, -1);

      float normal_length = 10.0f;
      size_t s = m->model_len->faces_count * 12;
      float* buffer = malloc(sizeof(float) * s);
      size_t buffer_counter = 0;

      vec4 color1 = COLOR_GREEN;
      vec4 color2 = COLOR_WHITE;

      for(size_t i = 0; i < m->model_len->faces_count; i++)
      {
         model_face_t* face = m->faces[i];
         assert(face->count != 3);

         vec4 normal = m->normals[face->normal_id[0] - 1];

         float x = 0, y = 0, z = 0;
         for(size_t j = 0; j < face->count; j++)
         {
            vec4 vert = m->vertices[face->vert_id[j] - 1];
            x += vert[0];
            y += vert[1];
            z += vert[2];
         }

         float x1 = x / face->count;
         float y1 = y / face->count;
         float z1 = z / face->count;

         float x2 = x1 + normal[0] * normal_length;
         float y2 = y1 + normal[1] * normal_length;
         float z2 = z1 + normal[2] * normal_length;

         buffer[buffer_counter++] = x1;
         buffer[buffer_counter++] = y1;
         buffer[buffer_counter++] = z1;

         buffer[buffer_counter++] = color1[0];
         buffer[buffer_counter++] = color1[1];
         buffer[buffer_counter++] = color1[2];

         buffer[buffer_counter++] = x2;
         buffer[buffer_counter++] = y2;
         buffer[buffer_counter++] = z2;

         buffer[buffer_counter++] = color2[0];
         buffer[buffer_counter++] = color2[1];
         buffer[buffer_counter++] = color2[2];
      }

      GLuint normal_vao;
      GLuint normal_vbo;

      GL_CALL(glGenVertexArrays(1, &normal_vao));
      GL_CALL(glGenBuffers(1, &normal_vbo));

      GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, normal_vbo));
      GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * s, buffer, GL_STATIC_DRAW));
      free(buffer);

      GL_CALL(glBindVertexArray(normal_vao));
      GL_CALL(glEnableVertexAttribArray(0));
      GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0));
      GL_CALL(glEnableVertexAttribArray(1));
      GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));

      GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
      GL_CALL(glBindVertexArray(0));

      object_t* obj = create_textured_dummy(
            vec3f(-150, 0, 0),
            0.3f, rm_getn(MATERIAL, "menu_spheres"), m);

      obj->draw_info->draw_normals = true;
      obj->draw_info->normal_vao = normal_vao;
      obj->draw_info->normal_buffer_len = s / 3;
      u_push_object(obj);

      first_map_load = false;
   }
}

object_t* create_game_map()
{
   object_t* this = o_create();
   this->update_func = game_map_update;
   return this;
}


