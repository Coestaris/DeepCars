//
// Created by maxim on 6/6/20.
//

#ifdef __GNUC__
#pragma implementation "creature.h"
#endif
#include "genome.h"
#include "../../lib/coredefs.h"

void gn_free(genome_t* gn)
{
   ASSERT(gn);
   DEEPCARS_FREE(gn->genes);
   if(gn->free_data_func && gn->data)
      gn->free_data_func(gn->data);
}

fitness_t gn_fitness(genome_t* gn)
{
   PASSERT(gn->fitness_func);

   if(gn->reset_func) gn->reset_func(gn);
   if(gn->step_func)
   {
      size_t time = 0;
      while (gn->step_func(gn, time++));
   }
   return gn->fitness = gn->fitness_func(gn);
}

void gn_mutate(genome_t* gn, float rate, bool gaussian)
{
   for(size_t i = 0; i < gn->genes_count; i++)
      gn->genes[i] += rand_range(0, 1) * (rate * 2) - rate;
}

void gn_crossover(genome_t* parent1, genome_t* parent2, genome_t* child1, genome_t* child2, crossover_t algorithm)
{
   PASSERT(parent1->genes_count == parent2->genes_count);
   child1->fitness = 0;
   child2->fitness = 0;

   if(parent1->clone_data_func && parent1->data)
   {
       parent1->clone_data_func(parent1->data, child1->data);
       parent1->clone_data_func(parent2->data, child2->data);
   }

   float a = rand_range(0.4, 0.6);
   for(size_t i = 0; i < parent1->genes_count; i++)
   {
      float p1 = parent1->genes[i];
      float p2 = parent2->genes[i];
      child1->genes[i] = a * p1 + (1 - a) * p2;
      child2->genes[i] = a * p2 + (1 - a) * p1;
   }
}

