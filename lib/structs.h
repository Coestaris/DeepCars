//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_STRUCTS_H
#define DEEPCARS_STRUCTS_H

#include <malloc.h>
#include <stddef.h>
#include <math.h>

// 2 dimensional floating point vector
typedef struct _vec2f
{
   double_t x;
   double_t y;

} vec2f_t;

// 3 dimensional floating point vector
typedef struct _vec3f
{
   double_t x;
   double_t y;
   double_t z;

} vec3f_t;

// Generic list to store pointers
typedef struct _list
{
   // Current count of elements in collection
   size_t count;
   // Actual size of allocated pointer
   size_t max_size;
   // Pointer to a data storing memory
   void** collection;

} list_t;

//
// List methods
//
// Push new element to a list
void list_push(list_t* list, void* object);

// Remove element from list by it's pointer
void list_remove(list_t* list, void* object);

// Free all lists elements
void list_free_elements(list_t* list);

// Free list but keep all its elements
void list_free(list_t* list);

// list_t constructor
list_t* list_create(int maxCount);

//
// Vector methods
//
// vec2f_t constructor
vec2f_t vec2f(double_t x, double_t y);

// vec3f_t constructor
vec3f_t vec3f(double_t x, double_t y, double_t z);


#endif //DEEPCARS_STRUCTS_H
