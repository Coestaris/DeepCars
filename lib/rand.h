//
// Created by maxim on 3/18/20.
//

#ifndef DEEPCARS_RAND_H
#define DEEPCARS_RAND_H

#include <stdint.h>
#include <stdbool.h>

#include "list.h"

float rand_f(void);
uint32_t rand_i(void);
bool rand_b(void);

void* rand_element(list_t* list);
float rand_range(float a, float b);
int32_t rand_rangei(int32_t a, int32_t b);

float rand_perlin2d(float x, float y, float freq, uint32_t depth);

#endif //DEEPCARS_RAND_H
