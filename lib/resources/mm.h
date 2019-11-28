//
// Created by maxim on 11/28/19.
//

#ifndef DEEPCARS_MM_H
#define DEEPCARS_MM_H

#include <stddef.h>
#include <stdbool.h>

#include "model.h"

// Inits all necessary mm resources
void mm_init(void);

// Frees all mm resources. if 'free_model' set to 1 also frees al its models
void mm_free(bool free_models);

// Returns the model with the specified identifier.
// If the model with the specified identifier cannot be found, NULL is returned
model_t* mm_get(size_t id);

// Adds new model to model manager
void mm_push(size_t id, model_t* model, bool build);

#endif //DEEPCARS_MM_H
