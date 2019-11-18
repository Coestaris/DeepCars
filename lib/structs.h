//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_STRUCTS_H
#define DEEPCARS_STRUCTS_H

#include <malloc.h>
#include <stddef.h>

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

typedef struct _list {
    size_t count;
    size_t maxSize;

    void** collection;
} list_t;

void listPush(list_t* list, void* object);
void listRemove(list_t* list, void* drawableObject);
void listFreeElements(list_t* list);
void listFree(list_t* list);
list_t* createList(int maxCount);

vec2f_t vec2f(double x, double y);
vec3f_t vec3f(double x, double y, double z);


#endif //DEEPCARS_STRUCTS_H
