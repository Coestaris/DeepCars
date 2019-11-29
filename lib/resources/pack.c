//
// Created by maxim on 11/27/19.
//

#ifdef __GNUC__
#pragma implementation "pack.h"
#endif
#include "pack.h"

#define P_LOG(format, ...) DC_LOG("pack.c", format, __VA_ARGS__)
#define P_ERROR(format, ...) DC_ERROR("pack.c", format, __VA_ARGS__)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <zlib.h>
#include "../../oil/crc32.h"
#include "mm.h"
#include "txm.h"
#include "../shaders/shader.h"
#include "../shaders/shm.h"

// Reads as many bytes as the specified parameter occupies.
// If reading failed, an error will be thrown
#define READ(param)                                                       \
   if(fread(&param, sizeof(param), 1, f) != 1)                            \
   {                                                                      \
      P_ERROR("Unable to read %s of size %li", #param, sizeof(param));    \
   }

#define READ_BUFF(buffer, cnt)                              \
      memcpy(&buffer, data + count, cnt); count += cnt;     \
      if(count > length)                                    \
      {                                                     \
         P_ERROR("Buffer overflow",0);                      \
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
      P_ERROR("Unable to decompress. ZLib error: %i", result);
      inflateEnd(&infstream);
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
   uint8_t norm_flags;
   uint32_t uncomp_data_len;

   READ_BUFF(id,sizeof(id));
   READ_BUFF(name_len,sizeof(name_len));

   char* name = malloc(name_len + 1);
   READ_BUFF(*name, name_len);
   name[name_len] = '\0';

   READ_BUFF(norm_flags, sizeof(norm_flags));
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
   m_normalize(m,
            norm_flags & 0x1u,
            norm_flags & 0x2u,
            norm_flags & 0x4u,
            norm_flags & 0x8u);
   mm_push(id, m, true);

   free(name);
   free(model_data);
}

// Handles shaders chunk and adds parsed shaders to shm
void p_handler_shader(uint8_t* data, size_t length)
{
   size_t count = 0;

   uint32_t id;
   uint8_t compress;
   uint8_t shader_flags;
   uint32_t name_len;

   READ_BUFF(id, sizeof(id));
   READ_BUFF(name_len,sizeof(name_len));

   char* name = malloc(name_len + 1);
   READ_BUFF(*name, name_len);
   name[name_len] = '\0';

   READ_BUFF(shader_flags, sizeof(shader_flags));
   READ_BUFF(compress, sizeof(compress));

   bool has_vertex = shader_flags & 0x1u;
   bool has_fragment = shader_flags & 0x2u;
   bool has_geometry = shader_flags & 0x4u;

   uint8_t* vertex_data    = NULL;
   uint8_t* fragment_data  = NULL;
   uint8_t* geometry_data  = NULL;
   uint32_t vertex_len     = 0;
   uint32_t fragment_len   = 0;
   uint32_t geometry_len   = 0;

   uint32_t uncomp_data_len;

   if(has_vertex)
   {
      READ_BUFF(vertex_len, sizeof(vertex_len));
      READ_BUFF(uncomp_data_len, sizeof(uncomp_data_len));
      vertex_data = malloc(vertex_len);
      READ_BUFF(*vertex_data, vertex_len);

      if(compress)
      {
         uint8_t* decompressed = malloc(uncomp_data_len);
         memset(decompressed, 0, uncomp_data_len);
         p_decompress(vertex_len, uncomp_data_len, vertex_data, decompressed);

         free(vertex_data);
         vertex_data = decompressed;
         vertex_len = uncomp_data_len;
      }
   }

   if(has_fragment)
   {
      READ_BUFF(fragment_len, sizeof(fragment_len));
      READ_BUFF(uncomp_data_len, sizeof(uncomp_data_len));
      fragment_data = malloc(fragment_len);
      READ_BUFF(*fragment_data, fragment_len);

      if(compress)
      {
         uint8_t* decompressed = malloc(uncomp_data_len);
         memset(decompressed, 0, uncomp_data_len);
         p_decompress(fragment_len, uncomp_data_len, fragment_data, decompressed);

         free(fragment_data);
         fragment_data = decompressed;
         fragment_len = uncomp_data_len;
      }
   }

   if(has_geometry)
   {
      READ_BUFF(geometry_len, sizeof(geometry_len));
      READ_BUFF(uncomp_data_len, sizeof(uncomp_data_len));
      geometry_data = malloc(geometry_len);
      READ_BUFF(*geometry_data, geometry_len);

      if(compress)
      {
         uint8_t* decompressed = malloc(uncomp_data_len);
         memset(decompressed, 0, uncomp_data_len);
         p_decompress(geometry_len, uncomp_data_len, geometry_data, decompressed);

         free(geometry_data);
         geometry_data = decompressed;
         geometry_len = uncomp_data_len;
      }
   }

   shader_t* sh = sh_create(strdup(name));
   sh_compile_s(sh,
           vertex_data, vertex_len,
           geometry_data, geometry_len,
           fragment_data, fragment_len);

   s_push(sh, id);

   if(vertex_data) free(vertex_data);
   if(fragment_data) free(fragment_data);
   if(geometry_data) free(geometry_data);
   free(name);
}

// Handles texture chunk and adds parsed textures to txm
void p_handler_texture(uint8_t* data, size_t length)
{
   size_t count = 0;

   uint32_t id;
   uint16_t name_len;
   uint32_t width;
   uint32_t heigth;
   uint8_t compression;
   uint8_t wrapping;
   uint8_t min;
   uint8_t mag;
   uint8_t flip;
   uint32_t data_len;

   READ_BUFF(id, sizeof(id));
   READ_BUFF(name_len, sizeof(name_len));

   char* name = malloc(name_len + 1);
   READ_BUFF(*name, name_len);
   name[name_len] = '\0';

   READ_BUFF(width, sizeof(width));
   READ_BUFF(heigth, sizeof(heigth));

   READ_BUFF(compression, sizeof(compression));
   READ_BUFF(wrapping, sizeof(wrapping));
   READ_BUFF(min, sizeof(min));
   READ_BUFF(mag, sizeof(mag));
   READ_BUFF(flip, sizeof(flip));

   READ_BUFF(data_len, sizeof(data_len));
   uint8_t* tex_data = malloc(data_len);
   READ_BUFF(*tex_data, data_len);

   texture_t* t = t_create(strdup(name));

   switch (wrapping)
   {
      case 1: t->data->wrappingMode = GL_MIRRORED_REPEAT;
         break;
      case 2: t->data->wrappingMode = GL_CLAMP_TO_EDGE;
         break;
      case 3: t->data->wrappingMode = GL_CLAMP_TO_BORDER;
         break;

      default:
      case 0: t->data->wrappingMode = GL_REPEAT;
         break;
   }

   switch (min)
   {
      case 1: t->data->minFilter = GL_NEAREST_MIPMAP_NEAREST;
         break;
      case 2: t->data->minFilter = GL_LINEAR;
         break;
      case 3: t->data->minFilter = GL_NEAREST;
         break;

      default:
      case 0: t->data->minFilter = GL_LINEAR_MIPMAP_LINEAR;
         break;
   }

   switch (mag)
   {
      case 1: t->data->magFilter = GL_NEAREST;
         break;

      default:
      case 0: t->data->magFilter = GL_LINEAR;
         break;
   }

   switch (flip)
   {
      case 1:
         t->data->flipX = true;
         t->data->flipY = false;
         break;
      case 2:
         t->data->flipX = false;
         t->data->flipY = true;
         break;
      case 3:
         t->data->flipX = true;
         t->data->flipY = true;
         break;
      default:
      case 0:
         t->data->flipX = false;
         t->data->flipY = false;
         break;
   }

   t_load_s(t, tex_data, data_len, compression == 0);
   txm_push(id, t);

   free(name);
   free(tex_data);
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
   P_LOG("Loading resource pack \"%s\"", name)
   FILE* f = fopen(name, "rb");
   if(!f)
   {
      P_ERROR("Unable to open file \"%s\"", name);
   }

   char magic_buffer[sizeof(magic_bytes) / sizeof(char)];
   READ(magic_buffer);

   for(size_t i = 0; i < sizeof(magic_buffer); i++)
      if(magic_buffer[i] != magic_bytes[i])
      {
         P_ERROR("Magic bytes doesn't match",0);
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
         P_ERROR("Unable to read data from file",0);
      }

      uint32_t calculated_crc = CRC32(buffer, ch_len, 0xFFFFFFFF);
      if(ch_crc != calculated_crc)
      {
         P_ERROR("Hashes doesn't match. Expected 0x%X but got 0x%X", ch_crc, calculated_crc);
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
         P_ERROR("Unknown chunk type %i", ch_type);
      }
      free(buffer);
   }
   fclose(f);
}