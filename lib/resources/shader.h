//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHADER_H
#define DEEPCARS_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#define GL_GLEXT_PROTOTYPES

#include "../coredefs.h"

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../graphics/gmath.h"
#include "../graphics/material.h"

// Storing all necessary shader information
typedef struct _shader
{
   // Name of current shader
   char* name;

   // OpenGL program ID
   GLint prog_id;

   // Flag field that contains programs of the shaders
   //    0x1 - has vertex program
   //    0x2 - has fragment program
   //    0x4 - has geometry program
   uint8_t programs;

} shader_t;

// shader_t constructor
shader_t* sh_create(char* name);

// compiles shader from source and loads it to a GPU memory
void sh_compile_s(shader_t* sh,
        uint8_t* vertex_source,   GLint vertex_len,
        uint8_t* geometry_source, GLint geometry_len,
        uint8_t* fragment_source, GLint fragment_len);

// compiles shader from files and loads it to a GPU memory
void sh_compile(shader_t* sh, char* vertex_path, char* geometry_path, char* fragment_path);

// Frees shader and all its resources
void sh_free(shader_t* sh);

//
// Sets uniform variable by its location
//
void sh_set_int(GLint location, int value);
void sh_set_float(GLint location, float value);
void sh_set_mat4(GLint location, mat4 value);
void sh_set_vec2v(GLint location, float a, float b);
void sh_set_vec2(GLint location, vec4 v);
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
void sh_nset_vec2v(shader_t* sh, const char* name, float a, float b);
void sh_nset_vec2(shader_t* sh, const char* name, vec4 v);
void sh_nset_vec3v(shader_t* sh, const char* name, float a, float b, float c);
void sh_nset_vec3(shader_t* sh, const char* name, vec4 v);
void sh_nset_vec4v(shader_t* sh, const char* name, float a, float b, float c, float d);
void sh_nset_vec4(shader_t* sh, const char* name, vec4 v);

// Use OpenGL shader. Pass NULL to disable shaders at all
void sh_use(shader_t* sh);

// Print some info about shader
void sh_info(shader_t* sh);

#pragma clang diagnostic pop

#endif //DEEPCARS_SHADER_H
