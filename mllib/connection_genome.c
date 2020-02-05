//
// Created by maxim on 2/1/20.
//

#ifdef __GNUC__
#pragma implementation "connection_genome.h"
#endif
#include "connection_genome.h"
#include <malloc.h>
#include <string.h>

connection_genome_t* cg_create(
      size_t in_node,
      size_t out_node,
      float weight,
      size_t innovation,
      bool disabled)
{
   connection_genome_t* genome = malloc(sizeof(connection_genome_t));
   genome->in_node = in_node;
   genome->out_node = out_node;
   genome->weight = weight;
   genome->innovation = innovation;
   genome->disabled = disabled;
   return genome;
}

inline void cg_free(connection_genome_t* genome)
{
   free(genome);
}

connection_genome_t* cg_clone(connection_genome_t* genome)
{
   connection_genome_t* copy = malloc(sizeof(connection_genome_t));
   memcpy(copy, genome, sizeof(connection_genome_t));
   return copy;
}

inline void cg_disable(connection_genome_t* genome)
{
   genome->disabled = true;
}