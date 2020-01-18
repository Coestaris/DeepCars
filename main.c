#include "app/loader.h"

#include "sig_handlers.h"

#include "lib/structs.h"

int main(int argc, char* argv[])
{
   /*list_t* l = list_create(10, 1010010, 10);
   for (int i = 0; i < 10; i++) {
      list_push(l, i);
   }
   list_free(l);*/
   register_sig_handlers();

   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();
}