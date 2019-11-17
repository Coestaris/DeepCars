//
// Created by maxim on 9/1/19.
//

#ifndef sym_GMATH_H
#define sym_GMATH_H

#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <assert.h>

typedef float* mat4;
typedef float* vec4;

vec4 cvec4(double x, double y, double z, double l);
void freeVec4(vec4 vec);
mat4 cmat4();
void freeMat4(mat4 mat);

void printVec4(vec4 vec);
void printMat4(mat4 mat);

//debug only!
void fillMat4(mat4 m,
              float a1, float a2, float a3, float a4,
              float b1, float b2, float b3, float b4,
              float c1, float c2, float c3, float c4,
              float d1, float d2, float d3, float d4);
void fillVec4(vec4 a, double x, double y, double z, double l);

void identityMat(mat4 m);
void rotateMat4Y(mat4 m, float angle);
void rotateMat4X(mat4 m, float angle);
void rotateMat4Z(mat4 m, float angle);

void translateMat(mat4 m, float x, float y, float z);
void scaleMat(mat4 m, float x, float y, float z);
void orthoMat(mat4 m, float n, float f, float r, float t);
void perspectiveMat(mat4 m, float n, float f, float r, float t);
void perspectiveFovMat(mat4 m, float angleOfView, float imageAspectRatio, float n, float f);

void vec4_cpy(vec4 dest, vec4 src);
vec4 vec4_ccpy(vec4 v);

void vec4_addf(vec4 v, float value);
void vec4_addv(vec4 v, vec4 value);
void vec4_subf(vec4 v, float value);
void vec4_subv(vec4 v, vec4 value);

void vec4_mulf(vec4 v, float value);
float vec4_scalar_mulv(vec4 a, vec4 b);
void vec4_cross(vec4 a, vec4 b);

float vec4_len(vec4 v);
void vec4_norm(vec4 v);
void vec4_mulm(vec4 v, mat4 m);

void mat4_addf(mat4 m, float v);
void mat4_addm(mat4 m, mat4 v);
void mat4_mulv(mat4 m, float v);
void mat4_mulm(mat4 m, mat4 v);

#endif //sym_GMATH_H
