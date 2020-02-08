//
// Created by maxim on 2/4/20.
//

#ifdef __GNUC__
#pragma implementation "innovation.h"
#endif
#include "innovation.h"
#include "connection_genome.h"

innovation_t innovation_counter;

innovation_t i_get(void)
{
   return innovation_counter++;
}

void i_recalc(void* ptr, size_t count)
{
   connection_genome_t* connections = ptr;
   innovation_t max_innovation = 0;
   for(size_t i = 0; i < count; i++)
   {
      connection_genome_t* connection = &connections[i];
      max_innovation = max_innovation > connection->innovation ? max_innovation : connection->innovation;
   }

   innovation_counter = max_innovation + 1;
}

void i_reset(void)
{
   innovation_counter = 0;
}


