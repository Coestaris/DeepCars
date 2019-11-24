//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHADER_H
#define DEEPCARS_SHADER_H

#define GL_GLEXT_PROTOTYPES

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "../graphics/gmath.h"
#include "../graphics/material.h"

// Storing all necessary shader information
typedef struct _shader
{
   // Path to a file with vertex shader
   char* vertex_path;

   // Path to a file with fragment shader
   char* fragment_path;

   // OpenGL program ID
   GLint prog_id;

   // Precalculated uniform locations
   GLint* uniform_locations;

} shader_t;

// shader_t constructor
shader_t* sh_create(char* vertexPath, char* fragmentPath);

// Frees shader and all its resources
void sh_free(shader_t* sh);

//
// Sets uniform variable by its location
//
void sh_set_int(GLint location, int value);
void sh_set_float(GLint location, float value);
void sh_set_mat4(GLint location, mat4 value);
void sh_set_vec3v(GLint location, float a, float b, float c);
void sh_set_vec3(GLint location, vec4 v);
void sh_set_vec4v(GLint location, float a, float b, float c, float d);
void sh_set_vec4(GLint location, vec4 v);

//
// Sets uniform variable by its name
//
void sh_nset_int(shader_t* sh, const char* name, int value);
void sh_nset_float(shader_t* sh, const char* name, float value);
void sh_nset_mat4(shader_t* sh, const char* name, mat4 value);
void sh_nset_vec3v(shader_t* sh, const char* name, float a, float b, float c);
void sh_nset_vec3(shader_t* sh, const char* name, vec4 v);
void sh_nset_vec4v(shader_t* sh, const char* name, float a, float b, float c, float d);
void sh_nset_vec4(shader_t* sh, const char* name, vec4 v);

// Use OpenGL shader. Pass NULL to disable shaders at all
void sh_use(shader_t* sh);

// Print some info about shader
void sh_info(shader_t* sh);

#endif //DEEPCARS_SHADER_H
