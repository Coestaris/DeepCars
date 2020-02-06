//
// Created by maxim on 2/6/20.
//

#ifdef __GNUC__
#pragma implementation "evaluator.h"
#endif

#include <assert.h>
#include "evaluator.h"
#include "rand_helpers.h"

evaluator_t* ev_create(size_t population_size, genome_t* orig)
{
   evaluator_t* evaluator = malloc(sizeof(evaluator_t));
   evaluator->evaluate_func = NULL;
   evaluator->genomes = list_create();
   evaluator->population_size = population_size;
   if(orig)
   {
      for(size_t i = 0; i < population_size; i++)
         list_push(evaluator->genomes, gn_clone(orig));
   }
   evaluator->species = malloc(sizeof(struct _species) * population_size);
   for(size_t i = 0; i < population_size; i++)
   {
      evaluator->species[i] = malloc(sizeof(species_t));
      evaluator->species[i]->mascot = NULL;
      evaluator->species[i]->members = list_create();
   }
   evaluator->species_count = 0;

   return evaluator;
}

void ev_free(evaluator_t* evaluator)
{
   for(size_t i = 0; i < evaluator->genomes->count; i++)
      gn_free(evaluator->genomes->collection[i]);
   list_free(evaluator->genomes);

   for(size_t i = 0; i < evaluator->population_size; i++)
   {
      list_free(evaluator->species[i]->members);
      free(evaluator->species[i]);
   }
   free(evaluator->species);
   free(evaluator);
}

const float c1 = 1;
const float c2 = 1;
const float c3 = 5;
const float DT = 5;

int32_t compare_genome(const void* p1, const void* p2)
{
   genome_t* g1 = p1;
   genome_t* g2 = p2;
   return (g2->fitness - g1->fitness) * 1000;
}

list_t* new_genomes = NULL;

species_t* ev_random_species(evaluator_t* ev)
{
   float weight_sum = 0;
   for(size_t i = 0; i < ev->species_count; i++)
   {
      species_t* species = ev->species[i];
      weight_sum += species->adjusted_score;
   }

   float r = gn_rand_float() * weight_sum;
   float count_weight = 0;
   for(size_t i = 0; i < ev->species_count; i++)
   {
      species_t* species = ev->species[i];
      count_weight += species->adjusted_score;
      if(count_weight >= r)
         return species;
   }

   assert(0);
}

genome_t* ev_random_genome(species_t* ev)
{
   float weight_sum = 0;
   for(size_t i = 0; i < ev->members->count; i++)
   {
      genome_t* genome = ev->members->collection[i];
      weight_sum += genome->fitness;
   }

   float r = gn_rand_float() * weight_sum;
   float count_weight = 0;
   for(size_t i = 0; i < ev->members->count; i++)
   {
      genome_t* genome = ev->members->collection[i];
      count_weight += genome->fitness;
      if(count_weight >= r)
         return genome;
   }

   assert(0);
}

void ev_evaluate(evaluator_t* evaluator)
{
   // reset spicies
   for(size_t i = 0; i < evaluator->species_count; i++)
   {
      species_t* species = evaluator->species[i];
      size_t mascot_index = rand() % species->members->count;
      species->mascot = species->members->collection[mascot_index];

      species->members->count = 0;
      list_push(species->members, species->mascot);
   }

   // Place genomes into species
   for(size_t i = 0; i < evaluator->genomes->count; i++)
   {
      genome_t* genome = evaluator->genomes->collection[i];
      bool found_species = false;

      for(size_t j = 0; j < evaluator->species_count; j++)
      {
         species_t* species = evaluator->species[j];
         if(gn_compatibility_distance(genome, species->mascot, c1, c2, c3) < DT)
         {
            list_push(species->members, genome);
            genome->species_id = species;
            found_species = true;
            break;
         }
      }

      // if there is no appropriate species for genome, make a new one
      if(!found_species)
      {
         evaluator->species[evaluator->species_count]->mascot = genome;
         //add himself to a members
         list_push(evaluator->species[evaluator->species_count]->members, genome);
         genome->species_id = evaluator->species[evaluator->species_count];
         evaluator->species_count++;
      }
   }

   // Remove unused species
   size_t empty_index = -1;
   bool found_empty;
   do
   {
      found_empty = false;
      for(size_t i = empty_index + 1; i < evaluator->species_count; i++)
      {
         if(evaluator->species[i]->members->count == 0)
         {
               empty_index = i;
               found_empty = true;
               break;
         }
      }

      if(found_empty)
         for(size_t i = evaluator->species_count - 1; i >= empty_index; i--)
            evaluator->species[i] = evaluator->species[i - 1];
      else break;
   }
   while(true);

   // Evaluate genomes and assign score
   for(size_t i = 0; i < evaluator->genomes->count; i++)
   {
      genome_t* genome = evaluator->genomes->collection[i];
      species_t* species = genome->species_id;

      // parallel stuff
      genome->fitness = evaluator->evaluate_func(genome);
      species->adjusted_score += genome->fitness / (float)species->members->count;
   }

   if(!new_genomes)
      new_genomes = list_create();

   // put best genomes from each species into next generation
   for(size_t i = 0; i < evaluator->species_count; i++)
   {
      species_t* species = evaluator->species[i];

      qsort(species->members->collection, species->members->count, sizeof(void*), compare_genome);
      list_push(new_genomes, gn_clone(species->members->collection[0]));
   }

   while(new_genomes->count < evaluator->population_size)
   {
      species_t* species = ev_random_species(evaluator);

      genome_t* p1 = ev_random_genome(species);
      genome_t* p2 = ev_random_genome(species);
      genome_t* child = NULL;

      if(p1->fitness > p2->fitness)
         child = gn_crossover(p1, p2);
      else
         child = gn_crossover(p2, p1);
   }

   for(size_t i = 0; i < evaluator->genomes->count; i++)
      gn_free(evaluator->genomes->collection[i]);

   evaluator->genomes = new_genomes;
}
