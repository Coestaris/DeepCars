//
// Created by maxim on 11/27/19.
//

#ifdef __GNUC__
#pragma implementation "pack.h"
#endif
#include "pack.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../oil/crc32.h"

// Reads as many bytes as the specified parameter occupies.
// If reading failed, an error will be thrown
#define READ(param)                                                                       \
   if(fread(&param, sizeof(param), 1, f) != 1)                                            \
   {                                                                                      \
      printf("[pack.c][ERROR]: Unable to read %s of size %li", #param, sizeof(param));    \
      exit(EXIT_FAILURE);                                                                 \
   }

struct _pack_chunk {
   size_t id;
   void (*handler)(uint8_t* data, size_t length);
};

// Handles model chunk and adds parsed models to mm
void p_handler_model(uint8_t* data, size_t length)
{

}

// Handles shaders chunk and adds parsed shaders to shm
void p_handler_shader(uint8_t* data, size_t length)
{

}

// Handles texture chunk and adds parsed textures to txm
void p_handler_texture(uint8_t* data, size_t length)
{

}

// Array of supported chunks
struct _pack_chunk supported_chunks[] = {
   {0, p_handler_model },
   {1, p_handler_shader },
   {2, p_handler_texture },
};

const size_t supported_chunks_count = sizeof(supported_chunks) / sizeof(struct _pack_chunk);

// Bytes from which the file must begin to be considered as a pack file
char magic_bytes[] = { 'D', 'P', 'A', 'C', 'K' };


//
// p_load()
//
void p_load(const char* name)
{
   FILE* f = fopen(name, "rb");
   if(!f)
   {
      printf("[pack.c][ERROR]: Unable to open file %s", name);
      exit(EXIT_FAILURE);
   }

   char magic_buffer[sizeof(magic_bytes) / sizeof(char)];
   READ(magic_buffer);

   for(size_t i = 0; i < sizeof(magic_buffer); i++)
      if(magic_buffer[i] != magic_bytes[i])
      {
         printf("[pack.c][ERROR]: Magic bytes doesn't match");
         exit(EXIT_FAILURE);
      }

   uint32_t ch_count = 0;
   READ(ch_count);

   for(size_t i = 0; i < ch_count; i++)
   {
      uint32_t ch_len;
      uint8_t ch_type;
      uint32_t ch_crc;

      READ(ch_len);
      READ(ch_type);
      READ(ch_crc);

      uint8_t* buffer = malloc(sizeof(uint8_t) * ch_len);
      if(fread(buffer, ch_len, 1, f) != 1)
      {
         printf("[pack.c][ERROR]: Unable to read data from file");
         exit(EXIT_FAILURE);
      }

      uint32_t calculated_crc = CRC32(buffer, ch_len, 0xFFFFFFFF);
      if(ch_crc != calculated_crc)
      {
         printf("[pack.c][ERROR]: Hashes doesn't match. Expected 0x%X but got 0x%X", ch_crc, calculated_crc);
         exit(EXIT_FAILURE);
      }

      bool found = false;
      for(size_t j = 0; j < supported_chunks_count; j++)
      {
         if(supported_chunks[j].id == ch_type)
         {
            (supported_chunks[j].handler)(buffer, ch_len);
            found = true;
            break;
         }
      }

      if(!found)
      {
         printf("[pack.c][ERROR]: Unknown chunk type %i", ch_type);
         exit(EXIT_FAILURE);
      }
      free(buffer);
   }

   fclose(f);
}