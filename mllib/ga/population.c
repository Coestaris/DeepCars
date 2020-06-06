//
// Created by maxim on 6/6/20.
//

#ifdef __GNUC__
#pragma implementation "population.h"
#endif
#include "population.h"
#include "../../lib/coredefs.h"

population_t* pop_create(void (*gen_create_func)(size_t index, genome_t* gen), size_t count)
{
   population_t* pop = DEEPCARS_MALLOC(sizeof(population_t));
   pop->genomes = DEEPCARS_MALLOC(sizeof(genome_t) * count);
   for(size_t i = 0; i < count; i++)
      gen_create_func(i, &pop->genomes[i]);
   pop->population_size = count;
   pop->unused_count = 0;
   return pop;
}

void pop_free(population_t* pop)
{
   for(size_t i = 0; i < pop->population_size; i++)
      gn_free(&pop->genomes[i]);
   DEEPCARS_FREE(pop->genomes);
   DEEPCARS_FREE(pop);
}

genome_t* pop_best(population_t* pop)
{
   
}

void pop_ev_fitness(population_t* pop)
{
   
}

void pop_ev_selection(population_t* pop, bool min_are_best, size_t save_count)
{
   
}

void pop_ev_crossover(population_t* pop, crossover_t algorithm, bool gaussian)
{
   
}

