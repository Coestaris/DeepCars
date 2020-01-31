//
// Created by maxim on 2/1/20.
//

#ifndef DEEPCARS_CONNECTION_GENOME_H
#define DEEPCARS_CONNECTION_GENOME_H

#include <stddef.h>
#include <stdbool.h>

typedef struct _connection_genome {
   size_t in_node;
   size_t out_node;
   float weight;

   size_t innovation;
   bool disabled;

} connection_genome_t;

connection_genome_t* cg_create(
      size_t in_node,
      size_t out_node,
      float weight,
      size_t innovation,
      bool disabled);
void cg_free(connection_genome_t* genome);
connection_genome_t* cg_copy(connection_genome_t* genome);
void cg_disable(connection_genome_t* genome);

#endif //DEEPCARS_CONNECTION_GENOME_H
