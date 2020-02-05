#include <time.h>
#include "oil/font.h"

#include "app/loader.h"
#include "sig_handlers.h"
#include "mllib/genome.h"
#include "mllib/connection_genome.h"
#include "mllib/innovation.h"
#include "mllib/rand_helpers.h"

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

   genome_t* gn1 = gn_create(3, 1, 1, false);
   list_push(gn1->connections, cg_create(0, 3, 0.7f, 1, false));
   list_push(gn1->connections, cg_create(1, 3, -0.5f, 2, true));
   list_push(gn1->connections, cg_create(2, 3, 0.5f, 3, false));
   list_push(gn1->connections, cg_create(1, 4, 0.6f, 4, false));
   list_push(gn1->connections, cg_create(4, 3, 0.4f, 5, false));
   list_push(gn1->connections, cg_create(0, 4, 0.6f, 8, false));
   i_recalc(gn1->connections);

   gn_set_seed(draw_seed);
   gn_write(gn1, "p1.bmp", font);

#define cg_create_d(a,b,c,d,e) cg_create(a-1, b-1, c,d,e)

   genome_t* gn2 = gn_create(3, 1, 2, false);
   list_push(gn2->connections, cg_create_d(1, 4, 0.7f, 1, false));
   list_push(gn2->connections, cg_create_d(2, 4, 0.5f, 2, true));
   list_push(gn2->connections, cg_create_d(3, 4, 0.5f, 3, false));
   list_push(gn2->connections, cg_create_d(2, 5, 0.6f, 4, false));
   list_push(gn2->connections, cg_create_d(5, 4, 0.4f, 5, true));
   list_push(gn2->connections, cg_create_d(5, 6, 0.6f, 6, false));
   list_push(gn2->connections, cg_create_d(6, 4, 0.6f, 7, false));
   list_push(gn2->connections, cg_create_d(3, 5, 0.6f, 9, false));
   list_push(gn2->connections, cg_create_d(1, 6, 0.6f, 10, false));
   i_recalc(gn2->connections);

   gn_compatibility_distance(gn1, gn2, 1, 1, 3);

   gn_set_seed(draw_seed);
   gn_write(gn2, "p2.bmp", font);

   genome_t* offspring = gn_crossover(gn2, gn1);

   gn_set_seed(draw_seed);
   gn_write(offspring, "off.bmp", font);

   oilFontFree(font);

/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/
}