//
// Created by maxim on 2/1/20.
//

#ifndef DEEPCARS_GENOME_H
#define DEEPCARS_GENOME_H

#include "../lib/structs.h"

typedef struct _genome {
   float fitness;

   list_t* connections;
   list_t* nodes;

} genome_t;

genome_t* gn_create(size_t in_count, size_t out_count);
//todo:
//void gn_mutate_link           create new connection
//void gn_mutate_node            creates new node
//void gn_mutate_switch            disables or enables connection
//void gn_mutate_weight_nudge       offset weight
//void gn_mutate_weight_random      sets random value to a weight

//void gn_crossover
//void gn_compatibility_distance
void gn_write(genome_t* genome, const char* fn);

#endif //DEEPCARS_GENOME_H
