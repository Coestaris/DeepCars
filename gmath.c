//
// Created by maxim on 4/30/19.
//

#include "gmath.h"

vec4 cvec4(double x, double y, double z, double l)
{
    vec4 a = malloc(sizeof(float) * 4);
    a[0] = x;
    a[1] = y;
    a[2] = z;
    a[3] = l;
    return a;
}

mat4 cmat4()
{
    mat4 m = malloc(sizeof(float) * 4 * 4);
    memset(m, 0, sizeof(float) * 4 * 4);
    return m;
}

void freeVec4(vec4 vec)
{
    free(vec);
}

void freeMat4(mat4 mat)
{
    free(mat);
}

void identityMat(mat4 m)
{
    memset(m, 0, sizeof(float) * 4 * 4);
    for (int i = 0; i < 4; i++)
        m[i * 4 + i] = 1;
}

void printVec4(vec4 vec)
{
    printf("Vec4[%.3f, %.3f, %.3f, %.3f]\n", vec[0], vec[1], vec[2], vec[3]);
}

void fillMat4(mat4 m,
              float a1, float a2, float a3, float a4,
              float b1, float b2, float b3, float b4,
              float c1, float c2, float c3, float c4,
              float d1, float d2, float d3, float d4)
{
    m[0] = a1;
    m[1] = a2;
    m[2] = a3;
    m[3] = a4;
    m[4] = b1;
    m[5] = b2;
    m[6] = b3;
    m[7] = b4;
    m[8] = c1;
    m[9] = c2;
    m[10] = c3;
    m[11] = c4;
    m[12] = d1;
    m[13] = d2;
    m[14] = d3;
    m[15] = d4;
}

void printMat4(mat4 mat)
{
    printf("Mat4 [\n     ");
    for (int i = 0; i < 16; i++)
        printf("%.3f%s", mat[i], (i != 0 && (i + 1) % 4 == 0) ? "\n     " : ", ");
    printf("]\n");
}

vec4 vec4_cpy(vec4 v)
{
    vec4 b = cvec4(v[0], v[1], v[2], v[3]);
    return b;
}

void vec4_addf(vec4 v, float value)
{
    for (int i = 0; i < 4; i++)
        v[i] += value;
}

void vec4_addv(vec4 v, const vec4 value)
{
    for (int i = 0; i < 4; i++)
        v[i] += value[i];
}

void vec4_mulf(vec4 v, float value)
{
    for (int i = 0; i < 4; i++)
        v[i] *= value;
}

float vec4_scalar_mulv(vec4 a, vec4 b)
{
    float v = 0;
    for (int i = 0; i < 4; i++)
        v += a[i] * b[i];
    return v;
}

void vec4_mulv(vec4 a, vec4 b)
{
    assert(0);
}

float vec4_len(vec4 v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

void vec4_norm(vec4 v)
{
    float len = vec4_len(v);
    vec4_mulf(v, 1 / len);
}

void vec4_mulm(vec4 v, mat4 m)
{
    float cpy[4] = {v[0], v[1], v[2], v[3]};
    for (int i = 0; i < 4; i++)
        v[i] = cpy[0] * m[i * 4] +
               cpy[1] * m[i * 4 + 1] +
               cpy[2] * m[i * 4 + 2] +
               cpy[3] * m[i * 4 + 3];
}

void mat4_addf(mat4 m, float v)
{
    for (int i = 0; i < 15; i++)
        m[i] += v;
}

void mat4_addm(mat4 m, mat4 v)
{
    for (int i = 0; i < 15; i++)
        m[i] += v[i];
}

void mat4_mulv(mat4 m, float v)
{
    for (int i = 0; i < 15; i++)
        m[i] *= v;
}

void mat4_mulm(mat4 m, mat4 v)
{
    float cpy[16];

    memcpy(cpy, m, sizeof(float) * 4 * 4);
    memset(m, 0, sizeof(float) * 4 * 4);

    for (int n = 0; n < 16; n++)
        for (int i = 0; i < 4; i++)
            m[n] += cpy[i + (n / 4) * 4] * v[i * 4 + n % 4];
}

void translateMat(mat4 m, float x, float y, float z)
{
    m[3] += x;
    m[7] += y;
    m[11] += z;
}

void orthoMath(mat4 m, float n, float f, float r, float t)
{
    fillMat4(m,
             1 / r,   0,     0,            0,
             0,       1 / t, 0,            0,
             0,       0,     -2 / (f - n), - (f + n) / (f - n),
             0 ,      0 ,    0,            1);
}

void scaleMat(mat4 m, float x, float y, float z)
{
    m[0] *= x;
    m[5] *= y;
    m[10] *= z;
}

void rotateMat4Z(mat4 m, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    m[0] = c;
    m[1] = -s;
    m[4] = s;
    m[5] = c;
    m[10] = 1;
    m[15] = 1;
}