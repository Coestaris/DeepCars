//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_STRUCTS_H
#define DEEPCARS_STRUCTS_H

typedef struct _vec2f
{
    double x;
    double y;
} vec2f_t;

typedef struct _vec3f
{
    double x;
    double y;
    double z;
} vec3f_t;

vec2f_t vec2f(double x, double y);
vec3f_t vec3f(double x, double y, double z);

#endif //DEEPCARS_STRUCTS_H
