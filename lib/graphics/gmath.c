//
// Created by maxim on 4/30/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#ifdef __GNUC__
#pragma implementation "gmath.h"
#endif
#include "gmath.h"

//
// cvec4()
//
vec4 cvec4(double_t x, double_t y, double_t z, double_t l)
{
   vec4 a = malloc(sizeof(float) * 4);
   vec4_fill(a, x, y, z, l);
   return a;
}

//
// vec4_fill()
//
inline void vec4_fill(vec4 a, double_t x, double_t y, double_t z, double_t l)
{
   a[0] = x;
   a[1] = y;
   a[2] = z;
   a[3] = l;
}

//
// cmat4()
//
mat4 cmat4()
{
   mat4 m = malloc(sizeof(float) * 4 * 4);
   memset(m, 0, sizeof(float) * 4 * 4);
   return m;
}

//
// vec4_free()
//
inline void vec4_free(vec4 vec)
{
   free(vec);
}

//
// mat4_free()
//
inline void mat4_free(mat4 mat)
{
   free(mat);
}

//
// mat4_identity()
//
inline void mat4_identity(mat4 m)
{
   m[0] =  1;   m[1] =  0;   m[2] =  0;   m[3] =  0;
   m[4] =  0;   m[5] =  1;   m[6] =  0;   m[7] =  0;
   m[8] =  0;   m[9] =  0;   m[10] = 1;   m[11] = 0;
   m[12] = 0;   m[13] = 0;   m[14] = 0;   m[15] = 1;
}

//
// vec4_print()
//
inline void vec4_print(vec4 vec)
{
   printf("Vec4[%.3f, %.3f, %.3f, %.3f]\n", vec[0], vec[1], vec[2], vec[3]);
}

//
// mat4_fill()
//
inline void mat4_fill(mat4 m,
                      float a1, float a2, float a3, float a4,
                      float b1, float b2, float b3, float b4,
                      float c1, float c2, float c3, float c4,
                      float d1, float d2, float d3, float d4)
{
   m[0] =  a1;   m[1] =  a2;   m[2] =  a3;   m[3] =  a4;
   m[4] =  b1;   m[5] =  b2;   m[6] =  b3;   m[7] =  b4;
   m[8] =  c1;   m[9] =  c2;   m[10] = c3;   m[11] = c4;
   m[12] = d1;   m[13] = d2;   m[14] = d3;   m[15] = d4;
}

//
// mat4_print()
//
inline void mat4_print(mat4 mat)
{
   printf("Mat4 [\n     ");
   for (int i = 0; i < 16; i++)
      printf("%.3f%s", mat[i], (i != 0 && (i + 1) % 4 == 0) ? "\n     " : ", ");
   printf("]\n");
}

//
// vec4_cpy()
//
inline void vec4_cpy(vec4 dest, vec4 src)
{
   dest[0] = src[0];
   dest[1] = src[1];
   dest[2] = src[2];
   dest[3] = src[3];
}

//
// vec4_ccpy()
//
vec4 vec4_ccpy(vec4 v)
{
   vec4 b = cvec4(v[0], v[1], v[2], v[3]);
   return b;
}

//
// vec4_addf()
//
inline void vec4_addf(vec4 v, float value)
{
   v[0] += value;
   v[1] += value;
   v[2] += value;
   v[3] += value;
}

//
// vec4_addv()
//
inline void vec4_addv(vec4 v, vec4 value)
{
   v[0] += value[0];
   v[1] += value[1];
   v[2] += value[2];
   v[3] += value[3];
}

//
// vec4_subf()
//
inline void vec4_subf(vec4 v, float value)
{
   v[0] -= value;
   v[1] -= value;
   v[2] -= value;
   v[3] -= value;
}

//
// vec4_subv()
//
inline void vec4_subv(vec4 v, vec4 value)
{
   v[0] -= value[0];
   v[1] -= value[1];
   v[2] -= value[2];
   v[3] -= value[3];
}

//
// vec4_mulf()
//
inline void vec4_mulf(vec4 v, float value)
{
   v[0] *= value;
   v[1] *= value;
   v[2] *= value;
   v[3] *= value;
}

//
// vec4_scalar_mulv()
//
inline float vec4_scalar_mulv(vec4 a, vec4 b)
{
   float v = 0;
   v += a[0] * b[0];
   v += a[1] * b[1];
   v += a[2] * b[2];
   v += a[3] * b[3];
   return v;
}

//
// vec4_cross()
//
inline void vec4_cross(vec4 a, vec4 b)
{
   float x, y, z;
   x = a[1] * b[2] - a[2] * b[1];
   y = a[2] * b[0] - a[0] * b[2];
   z = a[0] * b[1] - a[1] * b[0];
   a[0] = x;
   a[1] = y;
   a[2] = z;
   a[3] = 0;
}

//
// vec4_len()
//
float vec4_len(vec4 v)
{
   return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

//
// vec4_norm()
//
inline void vec4_norm(vec4 v)
{
   float len = vec4_len(v);
   vec4_mulf(v, 1 / len);
}

//
// vec4_mulm()
//
inline void vec4_mulm(vec4 v, mat4 m)
{
   float cpy[4] = {v[0], v[1], v[2], v[3]};
   for (int i = 0; i < 4; i++)
      v[i] = cpy[0] * m[i * 4] +
             cpy[1] * m[i * 4 + 1] +
             cpy[2] * m[i * 4 + 2] +
             cpy[3] * m[i * 4 + 3];
}

//
// mat4_addf()
//
inline void mat4_addf(mat4 m, float v)
{
   for (int i = 0; i < 15; i++)
      m[i] += v;
}

//
// mat4_addm()
//
inline void mat4_addm(mat4 m, mat4 v)
{
   for (int i = 0; i < 15; i++)
      m[i] += v[i];
}

//
// mat4_mulv()
//
inline void mat4_mulv(mat4 m, float v)
{
   for (int i = 0; i < 15; i++)
      m[i] *= v;
}

//
// mat4_mulm()
//
inline void mat4_mulm(mat4 m, mat4 v)
{
   //save copy of m
   float cpy[16] = {
           m[0], m[1], m[2], m[3],
           m[4], m[5], m[6], m[7],
           m[8], m[9], m[10], m[11],
           m[12], m[13], m[14], m[15],
   };

   for (int n = 0; n < 16; n++)
   {
      m[n] = 0;
      for (int i = 0; i < 4; i++)
         m[n] += cpy[i + (n / 4) * 4] * v[i * 4 + n % 4];
   }
}

//
// mat4_translate()
//
inline void mat4_translate(mat4 m, float x, float y, float z)
{
   m[3] += x;
   m[7] += y;
   m[11] += z;
}

//
// mat4_ortho()
//
inline void mat4_ortho(mat4 m, float n, float f, float r, float t)
{
   mat4_fill(m,
             1 / r, 0, 0, 0,
             0, 1 / t, 0, 0,
             0, 0, -2 / (f - n), -(f + n) / (f - n),
             0, 0, 0, 1);
}

//
// mat4_perspective_fov()
//
inline void mat4_perspective_fov(
        mat4 m,
        float angle_of_view, float ratio,
        float n, float f)
{
   float scale = tanf(angle_of_view * 0.5f * (float) M_PI / 180.0f) * n;

   float r = ratio * scale;
   float l = -r;
   float t = scale;
   float b = -t;

   mat4_perspective(m, n, f, r, t);
}

//
// mat4_perspective()
//
inline void mat4_perspective(mat4 m, float n, float f, float r, float t)
{
   mat4_fill(m,
             n / r, 0, 0, 0,
             0, n / t, 0, 0,
             0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
             0, 0, -1, 0);
}


//
// mat4_scale()
//
inline void mat4_scale(mat4 m, float x, float y, float z)
{
   m[0] *= x;
   m[5] *= y;
   m[10] *= z;
}

//
// mat4_rotate_x()
//
inline void mat4_rotate_x(mat4 m, float angle)
{
   float c = cosf(angle);
   float s = sinf(angle);
   m[0] =  1;   m[1] =  0;   m[2] =  0;    m[3] =  0;
   m[4] =  0;   m[5] =  c;   m[6] =  s;    m[7] =  0;
   m[8] =  0;   m[9] = -s;   m[10] = c;    m[11] = 0;
   m[12] = 0;   m[13] = 0;   m[14] = 0;    m[15] = 1;
}

//
// mat4_rotate_y()
//
inline void mat4_rotate_y(mat4 m, float angle)
{
   float c = cosf(angle);
   float s = sinf(angle);
   m[0] =  c;    m[1] =  0;    m[2] = -s;    m[3] =  0;
   m[4] =  0;    m[5] =  1;    m[6] =  0;    m[7] =  0;
   m[8] =  s;    m[9] =  0;    m[10] = c;    m[11] = 0;
   m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
}

//
// mat4_rotate_z()
//
inline void mat4_rotate_z(mat4 m, float angle)
{
   float c = cosf(angle);
   float s = sinf(angle);
   m[0] =  c;    m[1] = -s;    m[2] =  0;    m[3] =  0;
   m[4] =  s;    m[5] =  c;    m[6] =  0;    m[7] =  0;
   m[8] =  0;    m[9] =  0;    m[10] = 1;    m[11] = 0;
   m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
}

#pragma clang diagnostic pop