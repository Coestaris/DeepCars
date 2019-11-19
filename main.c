#include "app/loader.h"

int main(int argc, char* argv[])
{
   app_initGraphics();
   app_loadResources();

   app_run();

   app_fin();
}