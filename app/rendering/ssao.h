//
// Created by maxim on 1/5/20.
//

#ifndef DEEPCARS_SSAO_H
#define DEEPCARS_SSAO_H

#include "../../lib/resources/texture.h"
#include "../../lib/graphics/gmath.h"

texture_t* generate_noise(size_t n);
vec4* generate_kernel(size_t n);

#endif //DEEPCARS_SSAO_H
