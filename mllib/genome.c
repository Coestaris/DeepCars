//
// Created by maxim on 2/1/20.
//

#ifdef __GNUC__
#pragma implementation "genome.h"
#endif
#include "genome.h"

#include "../oil/bmp.h"
#include "../oil/graphics.h"
#include "../lib/graphics/gmath.h"
#include "connection_genome.h"
#include "node_genome.h"
#include "rand_helpers.h"

genome_t* gn_create(size_t in_count, size_t out_count, size_t hidden_count, bool link)
{
   genome_t* genome = malloc(sizeof(genome_t));
   genome->nodes = list_create();
   genome->connections = list_create();

   size_t index = 0;
   for(size_t i = 0; i < in_count; i++)
      list_push(genome->nodes, ng_create(INPUT, index++));
   for(size_t i = 0; i < out_count; i++)
      list_push(genome->nodes, ng_create(OUTPUT, index++));
   for(size_t i = 0; i < hidden_count; i++)
      list_push(genome->nodes, ng_create(HIDDEN, index++));

   if(link)
   {
      //in - hidden connections
      for (size_t i = 0; i < in_count; i++)
         for (size_t h = 0; h < hidden_count; h++)
         {
            list_push(genome->connections,
                      cg_create(i, h + in_count + out_count, 1, i_get(), false));
         }

      //hidden - out connections
      for (size_t h = 0; h < hidden_count; h++)
         for (size_t o = 0; o < out_count; o++)
         {
            list_push(genome->connections,
                      cg_create(h + in_count + out_count, o + in_count, 1, i_get(), false));
         }
   }

   return genome;
}

void gn_write(genome_t* genome, const char* fn, oilFont* font)
{
   bmpImage* image = oilBMPCreateImageExt(GN_WRITE_WIDTH, GN_WRITE_HEIGHT, 24, BITMAPINFOHEADER);
   if(!image) {
      oilPrintError();
      return;
   }

   float* positions = malloc(sizeof(float) * 2 * genome->nodes->count);
   size_t inputs = 0;
   size_t outputs = 0;

   for(size_t i = 0; i < genome->nodes->count; i++)
      switch(((node_genome_t*)genome->nodes->collection[i])->type)
      {
         case INPUT: inputs++; break;
         case OUTPUT: outputs++; break;
      }

   float input_mul = (GN_WRITE_WIDTH / (float)(inputs));
   float input_offset = input_mul / 2.0f;
   float output_mul = (GN_WRITE_WIDTH / (float)(outputs));
   float output_offset = output_mul / 2.0f;
   size_t input_i = 0;
   size_t output_i = 0;
   for(size_t i = 0; i < genome->nodes->count; i++)
   {
      node_genome_t* node = genome->nodes->collection[i];
      switch (node->type)
      {
         case INPUT:
            positions[i * 2] = (input_i++) * input_mul + input_offset;
            positions[i * 2 + 1] = GN_WRITE_HEIGHT - GN_WRITE_RADIUS - 5;
            break;
         case HIDDEN:
         {
            size_t tries = 0;
            while(true)
            {
               bool intersects = false;
               float new_x = (float) gn_rand_float() * (GN_WRITE_WIDTH - GN_WRITE_RADIUS * 6) + GN_WRITE_RADIUS * 3;
               float new_y = (float) gn_rand_float() * (GN_WRITE_HEIGHT - GN_WRITE_RADIUS * 6) + GN_WRITE_RADIUS * 3;;

               for (size_t j = 0; j < i; j++)
               {
                  node_genome_t* node = genome->nodes->collection[j];
                  if (node->type != HIDDEN) continue;

                  float node_x = positions[j * 2];
                  float node_y = positions[j * 2 + 1];

                  float dist = sqrtf(
                        (new_x - node_x) * (new_x - node_x) +
                        (new_y - node_y) * (new_y - node_y));

                  if (dist < GN_WRITE_RADIUS * 3)
                  {
                     intersects = true;
                     break;
                  }
               }

               positions[i * 2] = new_x;
               positions[i * 2 + 1] = new_y;

               tries++;
               assert(tries < 500);
               if(!intersects)
                  break;
            }
            break;
         }
         case OUTPUT:
            positions[i * 2] = (output_i++) * output_mul + output_offset;
            positions[i * 2 + 1] = GN_WRITE_RADIUS + 5;
            break;
      }
   }
   char buff[20];

   oilGrFill(image->colorMatrix, (oilColor){255,255,255});
   oilColor disabled_connection_color = {210, 210, 210};
   oilColor connection_color = {0, 0, 0};
   vec4 buff_vec = cvec4(0, 0, 0, 0);
   for(size_t i = 0; i < genome->connections->count; i++)
   {
      connection_genome_t* connection = genome->connections->collection[i];

      float x1 = positions[connection->in_node * 2];
      float y1 = positions[connection->in_node * 2 + 1];
      float x2 = positions[connection->out_node * 2];
      float y2 = positions[connection->out_node * 2 + 1];

      oilGrDrawLineSm(image->colorMatrix,
                    x1, y1,
                    x2, y2,
                    connection->disabled ? disabled_connection_color : connection_color);

      vec4_fill(buff_vec, x2 - x1, y2 - y1, 0, 0);
      vec4_norm(buff_vec);
      vec4_mulf(buff_vec, GN_WRITE_RADIUS + 2);

      float center_x = (float)x2 - buff_vec[0];
      float center_y = (float)y2 - buff_vec[1];
      if(center_x < 0) center_x = 0;
      else if(center_x > GN_WRITE_WIDTH) center_x = GN_WRITE_WIDTH - 1;
      if(center_y < 0) center_y = 0;
      else if(center_y > GN_WRITE_HEIGHT) center_y = GN_WRITE_HEIGHT - 1;

      oilGrDrawCircleSm(image->colorMatrix, center_x, center_y,
                      5, connection_color);

      oilGrFillCircle(image->colorMatrix, center_x, center_y,
                      4, connection_color);

      snprintf(buff, sizeof(buff), "%li", connection->innovation);
      oilGrDrawCenteredString(image->colorMatrix, font, buff, (x1 + x2) / 2, (y1 + y2) / 2, connection_color);
   }

   oilColor input_fill_color = {150, 0, 0};
   oilColor output_fill_color = {0, 150, 0};
   oilColor hidden_fill_color = {0, 150, 150};
   oilColor border_color = {0, 0, 0};
   for(size_t i = 0; i < genome->nodes->count; i++)
   {
      node_genome_t* node = genome->nodes->collection[i];
      oilColor* color;
      switch(node->type) {
         case INPUT: color = &input_fill_color; break;
         case HIDDEN: color = &hidden_fill_color; break;
         case OUTPUT: color = &output_fill_color; break;
      }
      oilGrFillCircle(image->colorMatrix, positions[i * 2], positions[i * 2 + 1],
            GN_WRITE_RADIUS, *color);
      oilGrDrawCircleSm(image->colorMatrix, positions[i * 2], positions[i * 2 + 1],
            GN_WRITE_RADIUS + 1, border_color);

      snprintf(buff, sizeof(buff), "%li", node->id + 1);
      oilGrDrawCenteredString(image->colorMatrix, font, buff, positions[i * 2], positions[i * 2 + 1] + 5, border_color);

   }

   if(!oilBMPSave(image, (char*)fn))
   {
      oilPrintError();
      return;
   }

   vec4_free(buff_vec);
   free(positions);
   oilBMPFreeImage(image);
}

genome_t* gn_crossover(genome_t* p1, genome_t* p2)
{
   genome_t* offspring = gn_create(0, 0, 0, false);
   for(size_t i = 0; i < p1->nodes->count; i++)
      list_push(offspring->nodes, ng_clone(p1->nodes->collection[i]));

   for(size_t i = 0; i < p1->connections->count; i++)
   {
      connection_genome_t* p1_connection = p1->connections->collection[i];
      connection_genome_t* p2_connection = NULL;

      //find p2 connection with same innovation
      for(size_t j = 0; j < p2->connections->count; j++)
      {
         connection_genome_t* connection = p2->connections->collection[j];
         if(connection->innovation == p1_connection->innovation)
         {
            p2_connection = connection;
            break;
         }
      }

      if(p2_connection)
      {
         // matching gene
         // randomly select one of them
         list_push(offspring->connections, cg_clone(gn_rand_float() > 0.5f ? p1_connection : p2_connection));
      }
      else
      {
         // disjoint or excess gene
         // get always from p1
         list_push(offspring->connections, cg_clone(p1_connection));
      }
   }

   return offspring;
}