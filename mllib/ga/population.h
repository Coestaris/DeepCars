//
// Created by maxim on 6/6/20.
//

#ifndef DEEPCARS_POPULATION_H
#define DEEPCARS_POPULATION_H

#include "genome.h"

typedef struct _population {
   genome_t* genomes;
   size_t population_size;

   size_t unused_count;

} population_t;

population_t* pop_create(void (*gen_create_func)(size_t index, genome_t* gen), size_t count);
void pop_free(population_t* pop);

genome_t* pop_best(population_t* pop);

void pop_ev_fitness(population_t* pop);
void pop_ev_selection(population_t* pop, bool min_are_best, size_t save_count);
void pop_ev_crossover(population_t* pop, crossover_t algorithm, bool gaussian);

#endif //DEEPCARS_POPULATION_H
