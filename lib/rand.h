//
// Created by maxim on 3/18/20.
//

#ifndef DEEPCARS_RAND_H
#define DEEPCARS_RAND_H

#include <stdint.h>
#include <stdbool.h>

#include "list.h"

// Use hash table instead of rand() function to get random integers
//#define USE_SIMPLE_RANDOM

// Returns random float value in range [0, 1]
float rand_f(void);

// Return random integer value in range [0, RAND_MAX]
uint32_t rand_i(void);

// Returns random bool (true or false, 1 or 0)
bool rand_b(void);

// Returns random element from list
void* rand_element(list_t* list);

// Returns random float number from specified range
float rand_range(float a, float b);

// Returns random integer number from specified range
int32_t rand_rangei(int32_t a, int32_t b);

// Returns value from 2 dimensional Perlin noise
float rand_perlin2d(float x, float y, float freq, size_t depth);

#endif //DEEPCARS_RAND_H
