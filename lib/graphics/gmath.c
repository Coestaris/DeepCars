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
vec4 cvec4(float_t x, float_t y, float_t z, float_t l)
{
   vec4 a = DEEPCARS_MALLOC(sizeof(float_t) * 4);
   vec4_fill(a, x, y, z, l);
   return a;
}

//
// vec4_fill()
//
inline void vec4_fill(vec4 a, float_t x, float_t y, float_t z, float_t l)
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
   mat4 m = DEEPCARS_MALLOC(sizeof(float_t) * 4 * 4);
   memset(m, 0, sizeof(float_t) * 4 * 4);
   return m;
}

//
// vec4_free()
//
inline void vec4_free(vec4 vec)
{
   DEEPCARS_FREE(vec);
}

//
// mat4_free()
//
inline void mat4_free(mat4 mat)
{
   DEEPCARS_FREE(mat);
}

inline void mat4_cpy(mat4 dest, mat4 src)
{
   dest[0]  = src[0];  dest[1] = src[1];  dest[2]  = src[2];  dest[3]  = src[3];
   dest[4]  = src[4];  dest[5] = src[5];  dest[6]  = src[6];  dest[7]  = src[7];
   dest[8]  = src[8];  dest[9] = src[9];  dest[10] = src[10]; dest[11] = src[11];
   dest[12] = src[12]; dest[13]= src[13]; dest[14] = src[14]; dest[15] = src[15];
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
                      float_t a1, float_t a2, float_t a3, float_t a4,
                      float_t b1, float_t b2, float_t b3, float_t b4,
                      float_t c1, float_t c2, float_t c3, float_t c4,
                      float_t d1, float_t d2, float_t d3, float_t d4)
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
// vec4_dist()
//
inline float vec4_dist(vec4 a, vec4 b)
{
   return sqrtf(
         (a[0] - b[0]) * (a[0] - b[0]) +
         (a[1] - b[1]) * (a[1] - b[1]) +
         (a[2] - b[2]) * (a[2] - b[2]) +
         (a[3] - b[3]) * (a[3] - b[3]));
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
   return cvec4(v[0], v[1], v[2], v[3]);
}

//
// vec4_addf()
//
inline void vec4_addf(vec4 v, float_t value)
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
inline void vec4_subf(vec4 v, float_t value)
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
inline void vec4_mulf(vec4 v, float_t value)
{
   v[0] *= value;
   v[1] *= value;
   v[2] *= value;
   v[3] *= value;
}

//
// vec4_scalar_mulv()
//
inline float_t vec4_scalar_mulv(vec4 a, vec4 b)
{
   float_t v = 0;
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
   float_t x, y, z;
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
float_t vec4_len(vec4 v)
{
   return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

//
// vec4_norm()
//
inline void vec4_norm(vec4 v)
{
   float_t len = vec4_len(v);
   vec4_mulf(v, 1 / len);
}

//
// vec4_mulm()
//
inline void vec4_mulm(vec4 v, mat4 m)
{
   float_t cpy[4] = {v[0], v[1], v[2], v[3]};
   for (int i = 0; i < 4; i++)
      v[i] = cpy[0] * m[i * 4] +
             cpy[1] * m[i * 4 + 1] +
             cpy[2] * m[i * 4 + 2] +
             cpy[3] * m[i * 4 + 3];
}

//
// mat4_addf()
//
inline void mat4_addf(mat4 m, float_t v)
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
inline void mat4_mulv(mat4 m, float_t v)
{
   for (int i = 0; i < 15; i++)
      m[i] *= v;
}

//
// mat4_mulm()
//
inline void mat4_mulm(mat4 m, mat4 v)
{
   float_t m0  = m[0],  m1  = m[1],  m2  = m[2],  m3  = m[3],
         m4  = m[4],  m5  = m[5],  m6  = m[6],  m7  = m[7],
         m8  = m[8],  m9  = m[9],  m10 = m[10], m11 = m[11],
         m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15];

   m[0] = m0 * v[0] + m1 * v[4] + m2 * v[8] + m3 * v[12];
   m[1] = m0 * v[1] + m1 * v[5] + m2 * v[9] + m3 * v[13];
   m[2] = m0 * v[2] + m1 * v[6] + m2 * v[10] + m3 * v[14];
   m[3] = m0 * v[3] + m1 * v[7] + m2 * v[11] + m3 * v[15];

   m[4] = m4 * v[0] + m5 * v[4] + m6 * v[8] + m7 * v[12];
   m[5] = m4 * v[1] + m5 * v[5] + m6 * v[9] + m7 * v[13];
   m[6] = m4 * v[2] + m5 * v[6] + m6 * v[10] + m7 * v[14];
   m[7] = m4 * v[3] + m5 * v[7] + m6 * v[11] + m7 * v[15];

   m[8] = m8 * v[0] + m9 * v[4] + m10 * v[8] + m11 * v[12];
   m[9] = m8 * v[1] + m9 * v[5] + m10 * v[9] + m11 * v[13];
   m[10] = m8 * v[2] + m9 * v[6] + m10 * v[10] + m11 * v[14];
   m[11] = m8 * v[3] + m9 * v[7] + m10 * v[11] + m11 * v[15];

   m[12] = m12 * v[0] + m13 * v[4] + m14 * v[8] + m15 * v[12];
   m[13] = m12 * v[1] + m13 * v[5] + m14 * v[9] + m15 * v[13];
   m[14] = m12 * v[2] + m13 * v[6] + m14 * v[10] + m15 * v[14];
   m[15] = m12 * v[3] + m13 * v[7] + m14 * v[11] + m15 * v[15];
}

//
// mat4_translate()
//
inline void mat4_translate(mat4 m, float_t x, float_t y, float_t z)
{
   m[3] += x;
   m[7] += y;
   m[11] += z;
}

//
// mat4_ortho()
//
inline void mat4_ortho(mat4 m, float_t n, float_t f, float_t r, float_t t)
{
   m[0] = 2 / r;
   m[1] = 0;
   m[2] = 0;
   m[3] = 0;

   m[4] = 0;
   m[5] = 2 / t;
   m[6] = 0;
   m[7] = 0;

   m[8] = 0;
   m[9] = 0;
   m[10] = -2.0f / (f - n);
   m[11] = -(f + n) / (f - n);

   m[12] = 0;
   m[13] = 0;
   m[14] = 0;
   m[15] = 1;
}

//
// mat4_perspective_fov()
//
inline void mat4_perspective_fov(
        mat4 m,
        float_t angle_of_view, float_t ratio,
        float_t n, float_t f)
{
   float_t scale = tanf(angle_of_view * 0.5f * (float_t) M_PI / 180.0f) * n;

   float_t r = ratio * scale;
   float_t t = scale;

   mat4_perspective(m, n, f, r, t);
}

//
// mat4_perspective()
//
inline void mat4_perspective(mat4 m, float_t n, float_t f, float_t r, float_t t)
{
   m[0] = n / r;
   m[1] = 0;
   m[2] = 0;
   m[3] = 0;

   m[4] = 0;
   m[5] = n / t;
   m[6] = 0;
   m[7] = 0;

   m[8] = 0;
   m[9] = 0;
   m[10] = -(f + n) / (f - n);
   m[11] = -2 * f * n / (f - n);

   m[12] = 0;
   m[13] = 0;
   m[14] = -1;
   m[15] = 0;
}


//
// mat4_scale()
//
inline void mat4_scale(mat4 m, float_t x, float_t y, float_t z)
{
   m[0] *= x;
   m[5] *= y;
   m[10] *= z;
}

//
// mat4_rotate()
//
inline void mat4_rotate(mat4 m, float_t angle, float_t x, float_t y, float_t z)
{
   float_t c = cosf(angle);    // cosine
   float_t s = sinf(angle);    // sine
   float_t c1 = 1.0f - c;                // 1 - c
   float_t m0 = m[0],  m4 = m[4],  m8 = m[8],  m12= m[12],
           m1 = m[1],  m5 = m[5],  m9 = m[9],  m13= m[13],
           m2 = m[2],  m6 = m[6],  m10= m[10], m14= m[14];

   // build rotation matrix
   float_t r0  = x * x * c1 + c;
   float_t r1  = x * y * c1 + z * s;
   float_t r2  = x * z * c1 - y * s;
   float_t r4  = x * y * c1 - z * s;
   float_t r5  = y * y * c1 + c;
   float_t r6  = y * z * c1 + x * s;
   float_t r8  = x * z * c1 + y * s;
   float_t r9  = y * z * c1 - x * s;
   float_t r10 = z * z * c1 + c;

   // multiply rotation matrix
   m[0]  = r0 * m0  + r4 * m1  + r8  * m2;
   m[1]  = r1 * m0  + r5 * m1  + r9  * m2;
   m[2]  = r2 * m0  + r6 * m1  + r10 * m2;
   m[4]  = r0 * m4  + r4 * m5  + r8  * m6;
   m[5]  = r1 * m4  + r5 * m5  + r9  * m6;
   m[6]  = r2 * m4  + r6 * m5  + r10 * m6;
   m[8]  = r0 * m8  + r4 * m9  + r8  * m10;
   m[9]  = r1 * m8  + r5 * m9  + r9  * m10;
   m[10] = r2 * m8  + r6 * m9  + r10 * m10;
   m[12] = r0 * m12 + r4 * m13 + r8  * m14;
   m[13] = r1 * m12 + r5 * m13 + r9  * m14;
   m[14] = r2 * m12 + r6 * m13 + r10 * m14;
}

//
// mat4_rotate_x()
//
inline void mat4_rotate_x(mat4 m, float_t angle)
{
   float_t c = cosf(angle);
   float_t s = sinf(angle);
   m[0] =  1;   m[1] =  0;   m[2] =  0;    m[3] =  0;
   m[4] =  0;   m[5] =  c;   m[6] =  s;    m[7] =  0;
   m[8] =  0;   m[9] = -s;   m[10] = c;    m[11] = 0;
   m[12] = 0;   m[13] = 0;   m[14] = 0;    m[15] = 1;
}

//
// mat4_rotate_y()
//
inline void mat4_rotate_y(mat4 m, float_t angle)
{
   float_t c = cosf(angle);
   float_t s = sinf(angle);
   m[0] =  c;    m[1] =  0;    m[2] = -s;    m[3] =  0;
   m[4] =  0;    m[5] =  1;    m[6] =  0;    m[7] =  0;
   m[8] =  s;    m[9] =  0;    m[10] = c;    m[11] = 0;
   m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
}

//
// mat4_rotate_z()
//
inline void mat4_rotate_z(mat4 m, float_t angle)
{
   float_t c = cosf(angle);
   float_t s = sinf(angle);
   m[0] =  c;    m[1] = -s;    m[2] =  0;    m[3] =  0;
   m[4] =  s;    m[5] =  c;    m[6] =  0;    m[7] =  0;
   m[8] =  0;    m[9] =  0;    m[10] = 1;    m[11] = 0;
   m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
}

#pragma clang diagnostic pop