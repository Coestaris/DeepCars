//
// Created by maxim on 3/5/20.
//

#ifndef DEEPCARS_VECTORS_H
#define DEEPCARS_VECTORS_H

#include <stddef.h>
#include <math.h>

// Represents empty vector (0, 1)
#define vec2e ((vec2){0,0})
// Represents unit vector (1, 1)
#define vec2u ((vec2){1,1})
// Represents vertical upwards vector (0, 1)
#define vec2up ((vec2){0,1})
// Represents vertical downwards vector (0, 1)
#define vec2down ((vec2){0,-1})
// Represents horizontal leftwards vector (0, 1)
#define vec2left ((vec2){1,0})
// Represents horizontal rightwards vector (0, 1)
#define vec2right ((vec2){-1,0})

// 2 dimensional floating point vector
typedef struct _vec2f
{
   double_t x;
   double_t y;

} vec2;

// 3 dimensional floating point vector
typedef struct _vec3f
{
   double_t x;
   double_t y;
   double_t z;

} vec3;

//
// Vector methods
//

// vec2 constructor
vec2 vec2f(double_t x, double_t y);

// vec2 constructor via two points (p = p2 - p1)
vec2 vec2fp(vec2 p1, vec2 p2);

// vec3 constructor
vec3 vec3f(double_t x, double_t y, double_t z);

// vec3 constructor via two points (p = p2 - p1)
vec3 vec3fp(vec3 p1, vec3 p2);

// Normalizes given vector
vec2 vec2_normalize(vec2 n);

// Distance between two points
double vec2_dist(vec2 a, vec2 b);

// Length of the given vector
double vec2_len(vec2 v);

// Normal to a given vector
vec2 vec2_normal(vec2 p);

// Dot product of two vectors
double vec2_dot(vec2 v1, vec2 v2);

// Linear interpolation between two vectors. X must be in range [0, 1]
vec2 vec2_lerp(vec2 a, vec2 b, double x);

#endif //DEEPCARS_VECTORS_H
