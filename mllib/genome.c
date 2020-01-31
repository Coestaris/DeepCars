//
// Created by maxim on 2/1/20.
//

#ifdef __GNUC__
#pragma implementation "genome.h"
#endif
#include "genome.h"
#include "node_genome.h"

genome_t* gn_create(size_t in_count, size_t out_count)
{
   genome_t* genome = malloc(sizeof(genome_t));
   genome->nodes = list_create();
   genome->connections = list_create();
   return genome;
}

