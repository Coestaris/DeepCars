//
// Created by maxim on 2/1/20.
//

#ifdef __GNUC__
#pragma implementation "genome.h"
#endif
#include "genome.h"
#include "node_genome.h"
#include "../oil/bmp.h"
#include "../oil/graphics.h"

genome_t* gn_create(size_t in_count, size_t out_count)
{
   genome_t* genome = malloc(sizeof(genome_t));
   genome->nodes = list_create();
   genome->connections = list_create();
   return genome;
}

#define GN_WRITE_WIDTH 512
#define GN_WRITE_HEIGHT 512
void gn_write(genome_t* genome, const char* fn)
{
   bmpImage* image = oilBMPCreateImageExt(GN_WRITE_WIDTH, GN_WRITE_HEIGHT, 24, BITMAPINFOHEADER);
   if(!image) {
      oilPrintError();
      return;
   }

   oilColor color = {0,0, 0, 0};
   oilGrFill(image->colorMatrix, color);

   if(!oilBMPSave(image, (char*)fn)) {
      oilPrintError();
      return;
   }
}
