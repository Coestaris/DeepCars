//
// Created by maxim on 2/4/20.
//

#ifdef __GNUC__
#pragma implementation "rand_helpers.h"
#endif
#include "rand_helpers.h"

node_genome_t* gn_rand_node(genome_t* genome)
{
   return genome->nodes->collection[rand() % genome->nodes->count];
}

connection_genome_t* gn_rand_connection(genome_t* genome)
{
   return genome->connections->collection[rand() % genome->connections->count];
}

void gn_set_seed(uint32_t seed)
{
   srand(seed);
}

float gn_rand_float(void)
{
   return (float)((uint32_t)rand()) / RAND_MAX;
}