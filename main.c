#include "app/loader.h"
#include "lib/resources/pack.h"
#include "lib/resources/txm.h"
#include "lib/resources/mm.h"
#include "lib/shaders/shm.h"

int main(int argc, char* argv[])
{
/*   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();*/

   mm_init();
   txm_init();
   s_init();


   p_load("resources.bin");

   mm_free(true);
   txm_free(true);
   s_free(true);
}