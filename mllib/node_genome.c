//
// Created by maxim on 2/1/20.
//

#ifdef __GNUC__
#pragma implementation "node_genome.h"
#endif
#include "node_genome.h"
#include <malloc.h>

node_genome_t* ng_create(node_type_t type, size_t id)
{
   node_genome_t* genome = malloc(sizeof(node_genome_t));
   genome->type = type;
   genome->id = id;
   return genome;
}

inline void ng_free(node_genome_t* genome)
{
   free(genome);
}

node_genome_t* ng_clone(node_genome_t* genome)
{
   return ng_create(genome->type, genome->id);
}