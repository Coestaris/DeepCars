//
// Created by maxim on 9/11/19.
//

#ifndef DEEPCARS_MODEL_H
#define DEEPCARS_MODEL_H

#define GL_GLEXT_PROTOTYPES

#include "../coredefs.h"

#include <GL/gl.h>
#include <errno.h>

#include "../graphics/gmath.h"
#include "../graphics/material.h"

// Max count of vertices per face
#define MAX_FACE_LEN 10

struct _model;

// Describes face of parent's model
typedef struct _model_face
{
   struct _model* parent;

   uint16_t mat_id;
   uint16_t sm_id;
   uint32_t group_flags;

   // Indices of vertices in parent model
   int32_t vert_id[MAX_FACE_LEN];
   // Indices of texture coordinates in parent model
   int32_t tex_id[MAX_FACE_LEN];
   // Indices of normals in parent model
   int32_t normal_id[MAX_FACE_LEN];

   // Count of vertices in face
   size_t count;

} model_face_t;

// Describes lengths and it's maximum length of vectors
// in parent model to implement dynamic array resizing
typedef struct _model_len
{
   // Actual count of elements in vectors
   size_t vertices_count;
   size_t normals_count;
   size_t tex_coords_count;
   size_t faces_count;
   size_t group_names_count;
   size_t mtl_libs_count;

   // Size of already allocated memory in that arrays
   size_t vertices_max_count;
   size_t normals_max_count;
   size_t tex_coords_max_count;
   size_t faces_max_count;
   size_t group_names_max_count;
   size_t mtl_libs_max_count;

} model_len_t;

// Representing all necessary data required for rendering 3D model
typedef struct _model
{
   // Path to the model file
   char* name;

   // List of all vertices used in this model
   vec4* vertices;
   // List of all normals used in this model
   vec4* normals;
   // List of all texture coordinates used in this model
   vec4* tex_coords;

   // All faces loaded in this model
   model_face_t** faces;

   // Name of this model
   char* object_name;

   // Group names in a model
   char** group_names;

   // Names of libs used in a model
   char** mtl_libs;

   // Length and maximum lengths of used vectors
   model_len_t* model_len;

   // OpenGL buffer objects
   GLuint VBO; // vertex buffer object
   GLuint VAO; // vertex array object
   GLuint EBO; // element buffer object (stores indices)

   // Actual size of triangles in model
   GLuint triangles_count;

} model_t;

// model_t constructor
model_t* m_create();

// Loads model from .obj formatted source text
model_t* m_load_s(char* name, char* source);

// Loads model from .obj file
model_t* m_load(const char* filename);

// Frees all resources used by model and deletes OpenGL buffers
void m_free(model_t* model);

// Prints some information about model primitives
void m_info(model_t* model);

// Allocates and uploads OpenGL buffer objects.
// This method should be called before rendering
void m_build(model_t* model);

// Creates model of 4-vertex plane
model_t* m_create_surface(uint32_t vpoly, uint32_t hpoly, bool global_uv, float (*height_func)(float x, float y));

// Moves the model to zero along the specified axes. Converts values to range of [0.0, 1.0]
// If norm_scale is set, scales the vertices of the model to the range of [1,0]
void m_normalize(model_t* model, bool norm_x_pos, bool norm_y_pos, bool norm_z_pos, bool norm_scale);

// Moves the model to zero along the specified axes. Converts values to range of [-0.5, 0.5]
// If norm_scale is set, scales the vertices of the model to the range of [1,0]
void m_normalize_sym(model_t* model, bool norm_x_pos, bool norm_y_pos, bool norm_z_pos, bool norm_scale);

// Calculates OpenGL vertex buffer storing normals of the model with
// specified length as set of lines
void m_calculate_normals_vao(model_t* model, vec4 color1, vec4 color2, float normal_len, GLuint* vao, size_t* len);

#endif //DEEPCARS_MODEL_H
