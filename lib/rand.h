//
// Created by maxim on 3/18/20.
//

#ifndef DEEPCARS_RAND_H
#define DEEPCARS_RAND_H

#include <stdint.h>
#include "list.h"

void* rand_element(list_t* list);
float rand_range(float a, float b);
float rand_f(void);
uint32_t rand_i(void);
float rand_perlin2d(float x, float y, float freq, uint32_t depth);

#endif //DEEPCARS_RAND_H
