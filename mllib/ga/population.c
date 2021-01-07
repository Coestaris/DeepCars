//
// Created by maxim on 6/6/20.
//

#ifdef __GNUC__
#pragma implementation "population.h"
#endif
#include "population.h"
#include "../../lib/coredefs.h"

#define POP_LOG(format, ...) DC_LOG("pop.c", format, __VA_ARGS__)
#define POP_ERROR(format, ...) DC_ERROR("pop.c", format, __VA_ARGS__)

population_t* pop_create(void (*gen_create_func)(size_t index, genome_t* gen), size_t count)
{
   population_t* pop = DEEPCARS_MALLOC(sizeof(population_t));
   pop->genomes = DEEPCARS_MALLOC(sizeof(genome_t) * count);
   pop->helper = DEEPCARS_MALLOC(sizeof(genome_t) * count);

#if DEBUG_LEVEL >= 2
   memset(pop->helper, 0, sizeof(genome_t) * count);
#endif

   for(size_t i = 0; i < count; i++)
      gen_create_func(i, &pop->genomes[i]);
   pop->population_size = count;
   pop->unused_count = 0;
   return pop;
}

void pop_print(population_t* pop)
{
   POP_LOG("Population contains %li genomes", 0);
}

void pop_free(population_t* pop)
{
   for(size_t i = 0; i < pop->population_size; i++)
      gn_free(&pop->genomes[i]);
   DEEPCARS_FREE(pop->genomes);
   DEEPCARS_FREE(pop);
}

genome_t* pop_best(population_t* pop, bool min_are_best, size_t* out_pos)
{
   size_t pos = 0;
   fitness_t best;
   if(min_are_best)
   {
      best = 9999999;
      for(size_t i = 0; i < pop->population_size; i++)
         if(pop->genomes[i].fitness < best)
         {
            best = pop->genomes[i].fitness;
            pos = i;
         }
   }
   else
   {
      best = -9999999;
      for(size_t i = 0; i < pop->population_size; i++)
         if(pop->genomes[i].fitness > best)
         {
            best = pop->genomes[i].fitness;
            pos = i;
         }
   }
   if(out_pos) *out_pos = pos;
   return &pop->genomes[pos];
}

void pop_ev_fitness(population_t* pop)
{
   for(size_t i = 0; i < pop->population_size; i++)
      gn_fitness(&pop->genomes[i]);
}

void pop_ev_selection(population_t* pop, bool min_are_best, size_t save_count)
{
   size_t helper_counter = 0;
   for(size_t i = 0; i < pop->population_size; i++)
   {
      size_t pos = 0;
      // Find best one and put it to a helper list, resetting fitness in original one
      genome_t* best = pop_best(pop, min_are_best, &pos);

      pop->helper[helper_counter++] = *best;
      best->fitness = min_are_best ? -9999999 : 9999999;
   }
   memcpy(pop->genomes, pop->helper, sizeof(genome_t) * pop->population_size);
#if DEBUG_LEVEL >= 2
   memset(pop->helper, 0, sizeof(genome_t) * pop->population_size);
#endif
}

void pop_ev_crossover(population_t* pop, crossover_t algorithm, bool gaussian)
{
   
}

