#include <time.h>
#include "oil/font.h"

#include "app/loader.h"
#include "sig_handlers.h"
#include "mllib/genome.h"
#include "mllib/connection_genome.h"
#include "mllib/innovation.h"
#include "mllib/rand_helpers.h"
#include "mllib/evaluator.h"
#include "mllib/network.h"

float evaluate_func(genome_t* genome)
{
   float input[2];
   float output[1];

   float fitness = 0;

   input[0] = 0; input[1] = 0;
   nw_forward(genome, input, output);
   fitness += 1 - output[0];

   input[0] = 1; input[1] = 0;
   nw_forward(genome, input, output);
   fitness += output[0];

   input[0] = 0; input[1] = 1;
   nw_forward(genome, input, output);
   fitness += output[0];

   input[0] = 1; input[1] = 1;
   nw_forward(genome, input, output);
   fitness += 1 - output[0];

   genome->fitness = fitness;
}

int main(int argc, char* argv[])
{
   if(!oilFontInit()) {
      oilPrintError();
      exit(1);
   }
   oilFont* font = oilFontLoad("../resources/fonts/arial.ttf",
         16, 20, 128);

   if(!font) {
      oilPrintError();
      exit(1);
   }

   register_sig_handlers();
   if(!oilFontFin()) {
      oilPrintError();
      exit(1);
   }

   gn_init_bank();

   uint32_t draw_seed = time(NULL);

   genome_t* orig_genome = gn_create(2, 1, 1, false);

   gn_push_connection(orig_genome, cg_create(0, 2, 0.7f, 1, false));
   gn_push_connection(orig_genome, cg_create(1, 2, -0.5f, 2, true));
   gn_push_connection(orig_genome, cg_create(1, 3, 0.6f, 4, false));
   gn_push_connection(orig_genome, cg_create(3, 2, 0.4f, 5, false));
   gn_push_connection(orig_genome, cg_create(0, 3, 0.6f, 8, false));
   i_recalc(orig_genome->connections, orig_genome->connections_count);
   gn_write(orig_genome, "image0.bmp", font);

   float in[2] = {(float) 1, (float )0};
   float out[1];
   nw_forward(orig_genome, in, out);

   evaluator_t* evaluator = ev_create(50, orig_genome, evaluate_func);
   genome_t* fittest = NULL;

   for(size_t i = 0; i < 500; i++)
   {
      gn_set_seed(time(NULL) + i);

      ev_mutate(evaluator);
      ev_evaluate(evaluator);

      fittest = ev_fittest_genome(evaluator);
      if(i % 2 == 0)
      {
         char buff[50];
         gn_set_seed(draw_seed);
         snprintf(buff, sizeof(buff), "image%li.bmp", i);
         gn_write(fittest, buff, font);
         printf("%li. Fitness: %.3lf, Species: %li\n", i ,fittest->fitness, evaluator->species_count);
      }
   }

   while(1)
   {
      int a, b;
      printf("Input two values:");
      scanf("%i %i", &a, &b);
      if(a > 1 || b > 1) break;

      float input[2] = {(float) a, (float )b};
      float output[1];

      nw_forward(fittest, input, output);
      printf("Result: %f\n", output[0]);
   }

   ev_free(evaluator);

   gn_free(orig_genome);

   gn_free_bank();

   oilFontFree(font);

/*
   gn_set_seed(draw_seed);
   gn_write(gn1, "p1.bmp", font);*/
/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/
}
