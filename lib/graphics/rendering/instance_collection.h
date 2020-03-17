//
// Created by maxim on 3/17/20.
//

#ifndef DEEPCARS_INSTANCE_COLLECTION_H
#define DEEPCARS_INSTANCE_COLLECTION_H

#include "../../resources/model.h"

typedef struct _instance_collection
{
   material_t* material;
   model_t* model;
   mat4* model_matrices;
   size_t amount;

} instance_collection_t;

instance_collection_t* ic_create(model_t* model, material_t* material, size_t amount);
void ic_free(instance_collection_t* ic, bool free_matrices);
void ic_set_mat(instance_collection_t* ic, size_t index, mat4 mat);

list_t* ic_get();

void ic_push(instance_collection_t* ic);
void ic_init(void);
void ic_release(bool free_collections, bool free_matrices);

#endif //DEEPCARS_INSTANCE_COLLECTION_H
