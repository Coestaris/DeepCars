//
// Created by maxim on 6/6/20.
//

#ifndef DEEPCARS_GENOME_H
#define DEEPCARS_GENOME_H

#include <stdbool.h>
#include <stddef.h>

typedef float fitness_t;

typedef enum {
   c_single_point,
   c_two_point,
   c_three_point,
   c_linear,
   c_blend,
   c_random_swap

} crossover_t;

typedef struct _genome {
   float* genes;
   size_t genes_count;

   fitness_t fitness;

   // Some OOP stuff
   void (*reset_func)(struct _genome*);
   bool (*step_func)(struct _genome*, size_t time);
   fitness_t (*fitness_func)(struct _genome*);
   void (*clone_data_func)(void* scr, void* dst);
   void* (*free_data_func)(void*);
   void* data;

} genome_t;

void gn_free(genome_t* gn);
fitness_t gn_fitness(genome_t* gn);
void gn_mutate(genome_t* gn, float rate, bool gaussian);
void gn_crossover(genome_t* parent1, genome_t* parent2, genome_t* child1, genome_t* child2, crossover_t algorithm);

#endif //DEEPCARS_GENOME_H
