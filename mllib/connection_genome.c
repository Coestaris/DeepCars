//
// Created by maxim on 2/1/20.
//

#ifdef __GNUC__
#pragma implementation "connection_genome.h"
#endif
#include "connection_genome.h"
#include <string.h>

connection_genome_t cg_create(
      size_t in_node,
      size_t out_node,
      float weight,
      size_t innovation,
      bool disabled)
{
   connection_genome_t genome =
   {
      .in_node = in_node,
      .out_node = out_node,
      .weight = weight,
      .innovation = innovation,
      .disabled = disabled
   };

   return genome;
}

connection_genome_t cg_clone(connection_genome_t* genome)
{
   return cg_create(genome->in_node, genome->out_node, genome->weight, genome->innovation, genome->disabled);
}

inline void cg_disable(connection_genome_t* genome)
{
   genome->disabled = true;
}