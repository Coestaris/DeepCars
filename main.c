#include <time.h>
#include "oil/font.h"

#include "app/loader.h"
#include "sig_handlers.h"
#include "mllib/genome.h"
#include "mllib/connection_genome.h"

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

   genome_t* gn = gn_create(3, 1, 1, false);
   list_push(gn->connections, cg_create(0, 3, 0.7f, 1, false));
   list_push(gn->connections, cg_create(1, 3, -0.5f, 2, true));
   list_push(gn->connections, cg_create(2, 3, 0.5f, 3, false));
   //list_push(gn->connections, cg_create(1, 4, 0.2f, 4, false));
   list_push(gn->connections, cg_create(4, 3, 0.4f, 5, false));
   list_push(gn->connections, cg_create(0, 4, 0.6f, 6, false));
   gn_innovation_recalc(gn);

   gn_set_seed(draw_seed);
   gn_write(gn, "image1.bmp", font);

   gn_set_seed(time(NULL) + 10);
   gn_mutate_link(gn);

   gn_set_seed(draw_seed);
   gn_write(gn, "image2.bmp", font);

   oilFontFree(font);

/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/
}