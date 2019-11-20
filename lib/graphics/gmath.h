//
// Created by maxim on 9/1/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#ifndef DEEPCARS_GMATH_H
#define DEEPCARS_GMATH_H

#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <assert.h>

typedef float* mat4; // 16 floats
typedef float* vec4; // 4 floats

// Creates new vector with specified 4 values
vec4 cvec4(double_t x, double_t y, double_t z, double_t l);

// Frees vector
void vec4_free(vec4 vec);

// Creates new empty matrix filled with 0
mat4 cmat4();

// Frees matrix
void mat4_free(mat4 mat);

// Creates copy of given vector
vec4 vec4_ccpy(vec4 v);

// Prints data structures
void vec4_print(vec4 vec);
void mat4_print(mat4 mat);

//
// Matrix arithmetic
//
// Adds given value to a matrix
void mat4_addf(mat4 m, float v);
// Adds given matrix to a matrix
void mat4_addm(mat4 m, mat4 v);
// Multiplies given value to a matrix
void mat4_mulv(mat4 m, float v);
// Multiplies given matrix to a matrix.
// Elementwise multiplication, not dot product
void mat4_mulm(mat4 m, mat4 v);

//
// Matrix transformations
//
// Rotates matrix around specified axis
void mat4_rotate_x(mat4 m, float angle);
void mat4_rotate_y(mat4 m, float angle);
void mat4_rotate_z(mat4 m, float angle);

// Translates matrix on specified values
void mat4_translate(mat4 m, float x, float y, float z);

// Scales matrix on specified values
void mat4_scale(mat4 m, float x, float y, float z);

// Fills matrix as identity matrix
void mat4_identity(mat4 m);

//
// Matrix fills
//
// Fills matrix with specified values
void mat4_fill(mat4 m,
               float a1, float a2, float a3, float a4,
               float b1, float b2, float b3, float b4,
               float c1, float c2, float c3, float c4,
               float d1, float d2, float d3, float d4);


// Fills matrix as orthogonal projection
void mat4_ortho(mat4 m, float n, float f, float r, float t);
// Fills matrix as perspective projection
void mat4_perspective(mat4 m, float n, float f, float r, float t);
// Fills matrix as perspective projection using Angle of view and Window size ratio
void mat4_perspective_fov(mat4 m, float angle_of_view, float ratio, float n, float f);

//
// Vector operations
//
// Copies values from one vector to another
void vec4_cpy(vec4 dest, vec4 src);

// Fills existing vector with specified values
void vec4_fill(vec4 a, double_t x, double_t y, double_t z, double_t l);

//
// Vector arithmetic
//
void vec4_addf(vec4 v, float value);
void vec4_addv(vec4 v, vec4 value);
void vec4_subf(vec4 v, float value);
void vec4_subv(vec4 v, vec4 value);
void vec4_mulf(vec4 v, float value);
float vec4_scalar_mulv(vec4 a, vec4 b);

// Vectors dot product. Result will be stored in vector 'a'
void vec4_cross(vec4 a, vec4 b);

// Length of vector
float vec4_len(vec4 v);

// Normalize given vector
void vec4_norm(vec4 v);

// Multiple vector by matrix
void vec4_mulm(vec4 v, mat4 m);

#endif //DEEPCARS_GMATH_H

#pragma clang diagnostic pop