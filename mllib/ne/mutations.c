/*
//
// Created by maxim on 2/4/20.
//
#include <assert.h>
#include "genome.h"
#include "connection_genome.h"
#include "rand_helpers.h"

void gn_mutate_node(genome_t* genome)
{
   connection_genome_t* connection;
   size_t tries = 0;

   while((connection = gn_rand_connection(genome))->disabled)
      if(tries++ > GN_MUTATE_NODE_MAX_TRIES)
      {
         puts("tries++ < GN_MUTATE_NODE_MAX_TRIES");
         return;
      }

   size_t id = genome->nodes_count;

   cg_disable(connection);

   gn_push_connection(
         genome,
         cg_create(connection->in_node, id, 1, i_get(), false));

   gn_push_connection(
         genome,
         cg_create(id, connection->out_node, connection->weight, i_get(), false));

   genome->nodes_count++;
}

void gn_mutate_switch(genome_t* genome, float chance)
{
   for(size_t i = 0; i < genome->connections_count; i++)
   {
      connection_genome_t* connection = &genome->connections[i];
      if(gn_rand_float() < chance)
         connection->disabled = !connection->disabled;
   }
}

void gn_mutate_weight_nudge(genome_t* genome, float chance, float rate)
{
   for(size_t i = 0; i < genome->connections_count; i++)
   {
      connection_genome_t* connection = &genome->connections[i];
      if(gn_rand_float() < chance)
      {
         connection->weight += gn_rand_float() * (connection->weight * rate) * (rand() % 2 ? 1.0f : -1.0f);
      }
   }
}

void gn_mutate_weight_random(genome_t* genome, float chance, float min, float max)
{
   for(size_t i = 0; i < genome->connections_count; i++)
   {
      connection_genome_t* connection = &genome->connections[i];
      if(gn_rand_float() < chance)
      {
         connection->weight = gn_rand_float() * (max - min) + min;
      }
   }
}

void gn_mutate_link(genome_t* genome)
{
   size_t tries = 0;
   while(true)
   {
      bool possible = false;
      bool exists = false;

      if(tries++ > GN_MUTATE_LINK_MAX_TRIES)
      {
         //puts("tries++ > GN_MUTATE_LINK_MAX_TRIES");
         return;
      }

      size_t i1 = rand() % genome->nodes_count;
      size_t i2 = rand() % genome->nodes_count;

      if (i1 != i2)
      {
         if (i1 < genome->input_count && i2 > genome->input_count + genome->output_count) // i1 - input, i2 - hidden
            possible = true;
         else if (i1 > genome->input_count + genome->output_count
               && i2 > genome->input_count && i2 < genome->input_count + genome->output_count) //i1 - hidden, i2 - output
            possible = true;
      }

      if (!possible)
         continue;

      for(size_t i = 0; i < genome->connections_count; i++)
      {
         connection_genome_t* connection = &genome->connections[i];
         if(connection->in_node == i1 && connection->out_node == i2)
         {
            exists = true;
            break;
         }
      }

      if(exists)
         continue;

      gn_push_connection(
            genome,
            cg_create(i1, i2, 1, i_get(), false));

      break;
   }
}*/
