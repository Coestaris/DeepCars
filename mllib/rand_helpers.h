//
// Created by maxim on 2/4/20.
//

#ifndef DEEPCARS_MLLIB_RAND_HELPERS_H
#define DEEPCARS_MLLIB_RAND_HELPERS_H

#include <stdint.h>
#include "genome.h"
#include "connection_genome.h"

float gn_rand_float(void);
void gn_set_seed(uint32_t seed);
connection_genome_t* gn_rand_connection(genome_t* genome);

#endif //DEEPCARS_MLLIB_RAND_HELPERS_H
