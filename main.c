#include "app/loader.h"
#include "sig_handlers.h"
#include "mllib/genome.h"

int main(int argc, char* argv[])
{
   register_sig_handlers();

   gn_write(NULL, "image.bmp");

/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/
}