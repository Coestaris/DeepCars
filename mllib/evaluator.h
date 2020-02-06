//
// Created by maxim on 2/6/20.
//

#ifndef DEEPCARS_EVALUATOR_H
#define DEEPCARS_EVALUATOR_H

#include "../lib/structs.h"
#include "genome.h"

typedef struct _evaluator {
   size_t population_size;
   list_t* genomes;

   float (*evaluate_func)(genome_t* genome);

   species_t** species;
   size_t species_count;

} evaluator_t;

evaluator_t* ev_create(size_t population_size, genome_t* orig);
void ev_evaluate(evaluator_t* evaluator);
void ev_free(evaluator_t* evaluator);

#endif //DEEPCARS_EVALUATOR_H
