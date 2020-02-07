#include <time.h>
#include "oil/font.h"

#include "app/loader.h"
#include "sig_handlers.h"
#include "mllib/genome.h"
#include "mllib/connection_genome.h"
#include "mllib/innovation.h"
#include "mllib/rand_helpers.h"
#include "mllib/evaluator.h"

float evaluate_func(genome_t* genome)
{
   size_t enabled_count = 0;
   for(size_t i = 0; i < genome->connections->count; i++)
      if(!((connection_genome_t *)genome->connections->collection[i])->disabled)
         enabled_count++;

   return 100.0f * enabled_count;
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

   uint32_t draw_seed = time(NULL);

   genome_t* orig_genome = gn_create(3, 1, 1, false);
   list_push(orig_genome->connections, cg_create(0, 3, 0.7f, 1, false));
   list_push(orig_genome->connections, cg_create(1, 3, -0.5f, 2, true));
   list_push(orig_genome->connections, cg_create(2, 3, 0.5f, 3, false));
   list_push(orig_genome->connections, cg_create(1, 4, 0.6f, 4, false));
   list_push(orig_genome->connections, cg_create(4, 3, 0.4f, 5, false));
   list_push(orig_genome->connections, cg_create(0, 4, 0.6f, 8, false));
   i_recalc(orig_genome->connections);

   evaluator_t* evaluator = ev_create(10, orig_genome, evaluate_func);

   for(size_t i = 0; i < 100; i++)
   {
      gn_set_seed(time(NULL) + i);

      ev_mutate(evaluator);
      ev_evaluate(evaluator);

      genome_t* fittest = ev_fittest_genome(evaluator);
      printf("Fitness: %.3lf, Species: %li\n", fittest->fitness, evaluator->species_count);
      if(i % 5 == 0)
      {
         char buff[50];
         gn_set_seed(draw_seed);
         snprintf(buff, sizeof(buff), "image%li.bmp", i);
         //gn_write(fittest, buff, font);
      }
   }

   ev_free(evaluator);

   gn_free(orig_genome);
   oilFontFree(font);

/*
   gn_set_seed(draw_seed);
   gn_write(gn1, "p1.bmp", font);*/
/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/
}
