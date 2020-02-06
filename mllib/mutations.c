//
// Created by maxim on 2/4/20.
//
#include <assert.h>
#include "genome.h"
#include "connection_genome.h"
#include "node_genome.h"
#include "rand_helpers.h"

void gn_mutate_node(genome_t* genome)
{
   connection_genome_t* connection;
   size_t tries;

   while((connection = gn_rand_connection(genome))->disabled)
      assert(tries++ > GN_MUTATE_NODE_MAX_TRIES);

   node_genome_t* new_node = ng_create(HIDDEN, genome->nodes->count);

   cg_disable(connection);

   list_push(genome->connections,
             cg_create(connection->in_node, new_node->id, 1, i_get(), false));
   list_push(genome->connections,
             cg_create(new_node->id, connection->out_node, connection->weight, i_get(), false));
   list_push(genome->nodes, new_node);
}

void gn_mutate_switch(genome_t* genome, float chance)
{
   for(size_t i = 0; i < genome->connections->count; i++)
   {
      connection_genome_t* connection = genome->connections->collection[i];
      if(gn_rand_float() < chance)
         connection->disabled = !connection->disabled;
   }
}

void gn_mutate_weight_nudge(genome_t* genome, float chance, float rate)
{
   for(size_t i = 0; i < genome->connections->count; i++)
   {
      connection_genome_t* connection = genome->connections->collection[i];
      if(gn_rand_float() < chance)
      {
         connection->weight += gn_rand_float() * (connection->weight * rate) * (rand() % 2 ? 1.0f : -1.0f);
      }
   }
}

void gn_mutate_weight_random(genome_t* genome, float chance, float min, float max)
{
   for(size_t i = 0; i < genome->connections->count; i++)
   {
      connection_genome_t* connection = genome->connections->collection[i];
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

      assert(tries++ < GN_MUTATE_LINK_MAX_TRIES);

      node_genome_t* node1 = gn_rand_node(genome);
      node_genome_t* node2 = gn_rand_node(genome);

      if (node1 != node2)
      {
         if (node1->type == INPUT && node2->type == HIDDEN)
            possible = true;
         else if (node1->type == HIDDEN && node2->type == OUTPUT)
            possible = true;
      }

      if (!possible)
         continue;

      for(size_t i = 0; i < genome->connections->count; i++)
      {
         connection_genome_t* connection = genome->connections->collection[i];
         if(connection->in_node == node1->id && connection->out_node == node2->id)
         {
            exists = true;
            break;
         }
      }

      if(exists)
         continue;

      list_push(genome->connections,
                cg_create(node1->id, node2->id, 1, i_get(), false));

      break;
   }
}