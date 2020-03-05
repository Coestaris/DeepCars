//
// Created by maxim on 2/1/20.
//

#ifndef DEEPCARS_MLLIB_GENOME_H
#define DEEPCARS_MLLIB_GENOME_H

#include <stdbool.h>
#include "../lib/coredefs.h"
#include "../oil/font.h"
#include "connection_genome.h"
#include "../lib/list.h"

#define GN_WRITE_WIDTH 512
#define GN_WRITE_HEIGHT 512
#define GN_WRITE_RADIUS 10

#define GN_MUTATE_NODE_MAX_TRIES 100
#define GN_MUTATE_LINK_MAX_TRIES 100

#define GN_COMATIBILITY_DISTANCE_THRESHOLD 25

#define GN_CREATE_STARTUP_MAX_CONNECTIONS 200
#define GN_CREATE_INCREASE_CONNECTIONS 1.5

struct _genome;

typedef struct _species
{
   struct _genome* mascot;
   list_t* members;
   float adjusted_score;
} species_t;


typedef struct _genome {
   float fitness;
   connection_genome_t* connections;
   size_t connections_count;
   size_t connections_max_count;

   size_t input_count;
   size_t output_count;
   size_t nodes_count;

   species_t* species_id;

   bool _free;

} genome_t;

void gn_init_bank(void);
void gn_free_bank(void);

genome_t* gn_create(size_t in_count, size_t out_count, size_t hidden_count, bool link);

void gn_push_connection(genome_t* genome, connection_genome_t connection_genome);

//creates a new random connection
void gn_mutate_link(genome_t* genome);
// creates a new random new node splitting connection by disabling it
void gn_mutate_node(genome_t* genome);
//disables or enables a random connection
void gn_mutate_switch(genome_t* genome, float chance);
//offset weight
void gn_mutate_weight_nudge(genome_t* genome, float chance, float rate);
//sets a random value to a random weight
void gn_mutate_weight_random(genome_t* genome, float chance, float min, float max);

//creates new genome from two parent
genome_t* gn_crossover(genome_t* p1, genome_t* p2);

//returns compatibility distance of two genomes
float gn_compatibility_distance(genome_t* p1, genome_t* p2, float c1, float c2, float c3);

void gn_write(genome_t* genome, const char* fn, oilFont* font);

void gn_free(genome_t* genome);

genome_t* gn_clone(genome_t* genome);

genome_t* gn_get_free_genome(void);

#endif //DEEPCARS_MLLIB_GENOME_H
