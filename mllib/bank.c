//
// Created by maxim on 2/9/20.
//

#ifdef __GNUC__
#pragma implementation "bank.h"
#endif
#include "genome.h"

list_t* genome_bank;

void gn_free_bank(void)
{
   for(size_t i = 0; i < genome_bank->count; i++)
   {
      genome_t* genome = genome_bank->collection[i];
      DEEPCARS_FREE(genome->connections);
      DEEPCARS_FREE(genome);
   }
   list_free(genome_bank);
}


genome_t* gn_alloc_new(void)
{
   genome_t* genome = DEEPCARS_MALLOC(sizeof(genome_t));
   genome->connections_max_count = GN_CREATE_STARTUP_MAX_CONNECTIONS;
   genome->connections_count = 0;
   genome->connections = DEEPCARS_MALLOC(sizeof(connection_genome_t) * genome->connections_max_count);
   genome->_free = true;

   genome->input_count = 0;
   genome->output_count = 0;
   genome->nodes_count = 0;
   genome->species_id = 0;
   genome->fitness = 0;

   return genome;
}

genome_t* gn_get_free_genome(void)
{
   for(size_t i = 0; i < genome_bank->count; i++)
   {
      genome_t* genome = genome_bank->collection[i];
      if(genome->_free)
      {
         genome->_free = false;
         return genome;
      }
   }
   // no DEEPCARS_FREE genomes found, create new one
   genome_t* genome =  gn_alloc_new();
   genome->_free = false;
   list_push(genome_bank, genome);
   return genome;
}


void gn_init_bank(void)
{
   genome_bank = list_create();
}



