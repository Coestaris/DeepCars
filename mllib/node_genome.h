//
// Created by maxim on 2/1/20.
//

#ifndef DEEPCARS_MLLIB_NODE_GENOME_H
#define DEEPCARS_MLLIB_NODE_GENOME_H

#include <stddef.h>

typedef enum _node_type {
   INPUT,
   HIDDEN,
   OUTPUT

} node_type_t;

typedef struct _node_genome {
   node_type_t type;
   size_t id;

} node_genome_t;

node_genome_t* ng_create(node_type_t type, size_t id);
void ng_free(node_genome_t* genome);

#endif //DEEPCARS_MLLIB_NODE_GENOME_H
