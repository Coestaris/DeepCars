#include "app/loader.h"
#include "sig_handlers.h"
#include "mllib/genome.h"
#include "mllib/connection_genome.h"

int main(int argc, char* argv[])
{
   register_sig_handlers();

   genome_t* gn = gn_create(3, 1, 1, false);
   list_push(gn->connections, cg_create(0, 3, 0.7f, 1, false));
   list_push(gn->connections, cg_create(1, 3, -0.5f, 2, true));
   list_push(gn->connections, cg_create(2, 3, 0.5f, 3, false));
   list_push(gn->connections, cg_create(1, 4, 0.2f, 4, false));
   list_push(gn->connections, cg_create(4, 3, 0.4f, 5, false));
   list_push(gn->connections, cg_create(0, 4, 0.6f, 6, false));
   list_push(gn->connections, cg_create(3, 4, 0.6f, 11, false));


   gn_write(gn, "image.bmp");

/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/
}