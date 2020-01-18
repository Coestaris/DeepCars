//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_chain.h"
#endif
#include "render_chain.h"
#include "../win.h"

render_chain_t* current_chain;

GLuint quad_vao;
GLuint quad_vbo;
GLuint quad_ebo;

GLuint cube_vao;
GLuint cube_vbo;

void rc_free(render_chain_t* rc, bool free_stages)
{
   if(free_stages)
   {
      for(size_t i = 0; i < rc->stages->count; i++)
         rs_free(rc->stages->collection[i]);
   }
   if(rc->data) free(rc->data);
   list_free(rc->stages);
   free(rc);
}

render_chain_t* rc_create()
{
   render_chain_t* this = malloc(sizeof(render_chain_t));
   this->data = NULL;
   this->stages = list_create();
   return this;
}

void rc_create_perspective(win_info_t* win, mat4 mat, float fov, float near, float far)
{
   mat4_perspective_fov(mat, fov, (float)win->w / (float)win->h, near, far);
}

void rc_create_ortho(win_info_t* win, mat4 mat, float near, float far)
{
   mat4_ortho(mat, near, far, win->w, win->h);
}

void rc_set_current(render_chain_t* rc)
{
   rc_link(rc);
   current_chain = rc;
}

render_chain_t* rc_get_current(void)
{
   return current_chain;
}

void setup_cube()
{
   float skyboxVertices[] = {
           // positions
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f,  1.0f
   };

   GL_CALL(glGenVertexArrays(1, &cube_vao));
   GL_CALL(glGenBuffers(1, &cube_vbo));
   GL_CALL(glBindVertexArray(cube_vao));
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, cube_vbo));

   GL_CALL(glBindVertexArray(cube_vao));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW));
   GL_CALL(glEnableVertexAttribArray(0));
   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
}

void setup_quad()
{
   float vertices[] = {
           1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
           1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
           -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
           -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
   };
   GLuint indices[] = {
           0, 1, 3,
           1, 2, 3
   };

   GL_CALL(glGenVertexArrays(1, &quad_vao));
   GL_CALL(glGenBuffers(1, &quad_vbo));
   GL_CALL(glGenBuffers(1, &quad_ebo));
   GL_CALL(glBindVertexArray(quad_vao));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, quad_vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo));
   GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
   GL_CALL(glEnableVertexAttribArray(0));

   GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
   GL_CALL(glEnableVertexAttribArray(1));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void rc_link(render_chain_t* rc)
{
   for(size_t i = 0; i < rc->stages->count; i++)
   {
      render_stage_t* stage = rc->stages->collection[i];
      if(i != 0)
         stage->prev_stage = rc->stages->collection[i - 1];
      else
         stage->prev_stage = NULL;
   }
}

void rc_build(render_chain_t* rc)
{
   for(size_t i = 0; i < rc->stages->count; i++)
   {
      render_stage_t* stage = rc->stages->collection[i];
      if(stage->render_mode == RM_GEOMETRY ||
         stage->render_mode == RM_FRAMEBUFFER ||
         stage->render_mode == RM_CUSTOM_FRAMEBUFFER)

         rs_build_tex(stage);
   }
}

GLint rc_get_quad_vao(void)
{
   if(!quad_vao) setup_quad();
   return quad_vao;
}

GLint rc_get_cube_vao(void)
{
   if(!cube_vao) setup_cube();
   return cube_vao;
}