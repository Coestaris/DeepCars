//
// Created by maxim on 3/5/20.
//

#ifdef __GNUC__
#pragma implementation "vectors.h"
#endif
#include "vectors.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

//
// vec2f
//
inline vec2 vec2f(double_t x, double_t y)
{
   return (vec2){x, y};
}

//
// vec2fp
//
inline vec2 vec2fp(vec2 p1, vec2 p2)
{
   return (vec2){p2.x - p1.x, p2.y - p1.y};
}

//
// vec3f
//
inline vec3 vec3f(double_t x, double_t y, double_t z)
{
   return (vec3){x, y, z};
}

//
// vec3fp
//
inline vec3 vec3fp(vec3 p1, vec3 p2)
{
   return (vec3){p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
}

//
// vec2_dist
//
inline double vec2_dist(vec2 a, vec2 b)
{
   return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

//
// vec2_len
//
inline double vec2_len(vec2 v)
{
   return sqrt(v.x * v.x + v.y * v.y);
}

//
// vec2_normalize
//
inline vec2 vec2_normalize(vec2 n)
{
   double n_len = vec2_len(n);
   n.x /= n_len;
   n.y /= n_len;
   return n;
}

//
// vec2_normal
//
inline vec2 vec2_normal(vec2 p)
{
   return vec2_normalize(vec2f(-p.y, p.x));
}

//
// vec2_dot
//
inline double vec2_dot(vec2 v1, vec2 v2)
{
   return v1.x * v2.x + v1.y * v2.y;
}

//
// vec2_lerp
//
inline vec2 vec2_lerp(vec2 a, vec2 b, double v)
{
   return vec2f(
         a.x * v + b.x * (1 - v),
         a.y * v + b.y * (1 - v));
}

#pragma clang diagnostic pop