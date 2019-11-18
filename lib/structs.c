//
// Created by maxim on 8/31/19.
//

#include "structs.h"

inline vec2f_t vec2f(double x, double y)
{
    vec2f_t v = {x, y};
    return v;
}

inline vec3f_t vec3f(double x, double y, double z)
{
    vec3f_t v = {x, y, z};
    return v;
}

void listPush(list_t* list, void* object)
{
    if(list->count > list->maxSize - 1)
    {
        size_t newLen = (int)((float)list->maxSize * 1.5f);
        list->collection = realloc(list->collection, sizeof(void*) * newLen);
        list->maxSize = newLen;
    }
    list->collection[list->count++] = object;
}

void listFreeElements(list_t* list)
{
    for(size_t i = 0; i < list->count; i++)
        free(list->collection[i]);
    list->count = 0;
}

void listFree(list_t* list)
{
    //listFreeElements(list);
    free(list->collection);
    free(list);
}

void listRemove(list_t* list, void* drawableObject)
{
    //todo
}

list_t* createList(int maxCount)
{
    list_t* list = malloc(sizeof(list_t));
    list->count = 0;
    list->maxSize = maxCount;
    list->collection = malloc(sizeof(void*) * maxCount);
    return list;
}