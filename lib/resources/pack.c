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
#include <string.h>
#include <zlib.h>
#include "../../oil/crc32.h"
#include "mm.h"

// Reads as many bytes as the specified parameter occupies.
// If reading failed, an error will be thrown
#define READ(param)                                                                       \
   if(fread(&param, sizeof(param), 1, f) != 1)                                            \
   {                                                                                      \
      printf("[pack.c][ERROR]: Unable to read %s of size %li", #param, sizeof(param));    \
      exit(EXIT_FAILURE);                                                                 \
   }

#define READ_BUFF(buffer, cnt)                              \
      memcpy(&buffer, data + count, cnt); count += cnt;     \
      if(count > length)                                   \
      {                                                     \
         printf("[pack.c][ERROR]: Buffer overflow");        \
         exit(EXIT_FAILURE);                                \
      }

struct _pack_chunk {
   size_t id;
   void (*handler)(uint8_t* data, size_t length);
};

void p_decompress(size_t in_len, size_t out_len, uint8_t* buff_in, uint8_t* buff_out)
{
   z_stream infstream;
   infstream.zalloc = Z_NULL;
   infstream.zfree = Z_NULL;
   infstream.opaque = Z_NULL;

   infstream.avail_in = (uInt)in_len;
   infstream.next_in = buff_in;
   infstream.avail_out = (uInt)out_len - 1;
   infstream.next_out = buff_out;

   inflateInit(&infstream);

   int result;
   if ((result = inflate(&infstream, Z_NO_FLUSH)) != Z_OK)
   {
      printf("[pack.c][ERROR]: Unable to decompress. ZLib error: %i", result);
      inflateEnd(&infstream);
      exit(EXIT_FAILURE);
   }

   inflateEnd(&infstream);
}

// Handles model chunk and adds parsed models to mm
void p_handler_model(uint8_t* data, size_t length)
{
   size_t count = 0;

   uint32_t id;
   uint16_t name_len;
   uint8_t arhive;
   uint32_t data_len;
   uint32_t uncomp_data_len;

   READ_BUFF(id,sizeof(id));
   READ_BUFF(name_len,sizeof(name_len));

   char* name = malloc(name_len + 1);
   READ_BUFF(*name, name_len);
   name[name_len] = '\0';

   READ_BUFF(arhive, sizeof(arhive));
   READ_BUFF(data_len, sizeof(data_len));
   READ_BUFF(uncomp_data_len, sizeof(uncomp_data_len));

   uint8_t* model_data = malloc(data_len);
   READ_BUFF(*model_data, data_len);

   if(arhive)
   {
      uint8_t* decompressed = malloc(uncomp_data_len);
      memset(decompressed, 0, uncomp_data_len);

      p_decompress(data_len, uncomp_data_len, model_data, decompressed);

      free(model_data);
      model_data = decompressed;
   }

   model_t* m = m_load_s(strdup(name), (char*)model_data);
   mm_push(id, m, true);

   free(name);
   free(model_data);
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