//
// Created by maxim on 2/9/20.
//

#ifdef __GNUC__
#pragma implementation "network.h"
#endif
#include "network.h"

struct _node
{
   int id;
   float activation;
   //float bias; ???
   list_t* input_connections;
   bool calculated;
};

struct _node* nodes = NULL;
size_t nodes_count;

float relu(float x)
{
   return x > 0 ? x : 0;
}

float sigmoid(float x)
{
   return 1.0f / (1.0f + expf(-x));
}

#define ACTIVATION_FUNC(x) sigmoid(x)
#define NW_FORWARD_NODES_INCREASE 1.5

void nw_rec_forward(struct _node* node, genome_t* genome)
{
   float sum = 0;
   for(size_t i = 0; i < node->input_connections->count; i++)
   {
      connection_genome_t* in_connection = node->input_connections->collection[i];
      struct _node* target_node = &nodes[in_connection->in_node];

      if(!target_node->calculated)
         nw_rec_forward(target_node, genome);

      sum += target_node->activation;
   }

   node->activation = ACTIVATION_FUNC(sum);
   node->calculated = true;
}

void nw_forward(genome_t* genome, float* input, float* output)
{
   if(!nodes)
   {
      nodes = DEEPCARS_MALLOC(sizeof(struct _node) * genome->nodes_count);
      memset(nodes, 0, sizeof(struct _node) * genome->nodes_count);
   }
   else
   {
      if(nodes_count < genome->nodes_count)
      {
         size_t new_size = genome->nodes_count * NW_FORWARD_NODES_INCREASE;
         nodes = realloc(nodes, sizeof(struct _node) * new_size);
         memset(nodes + nodes_count, 0,
                (new_size - nodes_count) * sizeof(struct _node));

         nodes_count = new_size;
      }
   }

   for(size_t i = 0; i < genome->nodes_count; i++)
   {
      nodes[i].activation = 0;
      nodes[i].calculated = false;
      if(!nodes[i].input_connections)
         nodes[i].input_connections = list_create();
      nodes[i].input_connections->count = 0;

      nodes[i].id = i + 1;
   }

   for(size_t i = 0; i < genome->input_count; i++)
   {
      nodes[i].activation = input[i];
      nodes[i].calculated = true;
   }
   for(size_t i = 0; i < genome->nodes_count; i++)
   {
      struct _node* node = &nodes[i];
      for(size_t j = 0; j < genome->connections_count; j++)
      {
         if(!genome->connections[j].disabled && genome->connections[j].out_node == i)
         {
            connection_genome_t* conection = &genome->connections[j];
            list_push(node->input_connections, conection);
         }
      }
   }

   for(size_t i = 0; i < genome->output_count; i++)
   {
      struct _node* out_node = &nodes[i + genome->input_count];
      nw_rec_forward(out_node, genome);

      output[i] = out_node->activation;
   }
}
