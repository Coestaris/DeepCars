#include "app/loader.h"

int main(int argc, char* argv[])
{
   app_init_graphics();
   app_load_resources();

   app_run();

   app_fin();
}