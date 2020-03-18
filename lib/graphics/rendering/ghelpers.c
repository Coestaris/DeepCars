//
// Created by maxim on 3/14/20.
//
#ifdef __GNUC__
#pragma implementation "graphics.h"
#endif
#include "graphics.h"

#include "../gmath.h"
#include "../win.h"

static GLuint quad_vbo;
static GLuint quad_vao;
static GLuint quad_ebo;
static GLuint cube_vao;
static GLuint cube_vbo;

static void setup_cube(void)
{
   float skybox_vertices[] =
   {
         -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,
         -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
   };

   GL_CALL(glGenVertexArrays(1, &cube_vao));
   GL_CALL(glGenBuffers(1, &cube_vbo));
   GL_CALL(glBindVertexArray(cube_vao));
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, cube_vbo));

   GL_CALL(glBindVertexArray(cube_vao));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW));
   GL_CALL(glEnableVertexAttribArray(0));
   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
}

static void setup_quad(void)
{
   float quad_vertices[] =
   {
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
   };
   GLuint indices[] =
   {
         0, 1, 3,
         1, 2, 3
   };

   GL_CALL(glGenVertexArrays(1, &quad_vao));
   GL_CALL(glGenBuffers(1, &quad_vbo));
   GL_CALL(glGenBuffers(1, &quad_ebo));
   GL_CALL(glBindVertexArray(quad_vao));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, quad_vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW));

   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo));
   GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0));
   GL_CALL(glEnableVertexAttribArray(0));

   GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float))));
   GL_CALL(glEnableVertexAttribArray(1));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
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

void rc_create_perspective(win_info_t* win, mat4 mat, float fov, float near, float far)
{
   mat4_perspective_fov(mat, fov, (float)win->w / (float)win->h, near, far);
}

void rc_create_ortho(win_info_t* win, mat4 mat, float near, float far)
{
   mat4_ortho(mat, near, far, win->w, win->h);
}