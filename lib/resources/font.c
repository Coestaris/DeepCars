//
// Created by maxim on 1/16/20.
//

#ifdef __GNUC__
#pragma implementation "font.h"
#endif
#include "font.h"

#define F_LOG(format, ...) DC_LOG("font.c", format, __VA_ARGS__)
#define F_ERROR(format, ...) DC_ERROR("font.c", format, __VA_ARGS__)

uint8_t read8(uint8_t** ptr)
{
   return *((*ptr)++);
}

uint16_t read16(uint8_t** ptr)
{
   return read8(ptr) | read8(ptr) << 8U;
}

uint32_t read32(uint8_t** ptr)
{
   return
      read8(ptr) << 0U |
      read8(ptr) << 8U |
      read8(ptr) << 16U|
      read8(ptr) << 24U;
}

font_t* f_create(char* name, texture_t* texture, shader_t* shader, uint8_t* info, size_t infolen)
{
   font_t* f = malloc(sizeof(font_t));
   f->name = name;
   f->texture = texture;
   f->shader = shader;

   uint8_t signature[3] = { read8(&info), read8(&info), read8(&info) };
   if(signature[0] != 'B' || signature[1] != 'M' || signature[2] != 'F')
      F_ERROR("BMF signatures doesn't match",0);

   size_t version = read8(&info);

   size_t block_id = read8(&info);
   if(block_id != 1)
      F_ERROR("Expected block with id 1 (info)",0);

   size_t info_size = read32(&info);
   info += info_size; // skip info block

   block_id = read8(&info);
   if(block_id != 2)
      F_ERROR("Expected block with id 2 (common)",0);
   size_t common_size = read32(&info);
   f->line_height = read16(&info) / FONT_SCALE;
   f->base = read16(&info) / FONT_SCALE;
   info += 4; //skip scales
   size_t pages = read16(&info);
   info += 5; //skip other useless common data
   if(pages != 1)
      F_ERROR("Using more than 1 page is not supported",0);

   block_id = read8(&info);
   if(block_id != 3)
      F_ERROR("Expected block with id 3 (pages)",0);
   size_t pages_size = (read8(&info) - 48) * 10 + (read8(&info) - 48); //given values are in ascii...
   info += pages_size; //skip pages info

   block_id = read8(&info);
   if(block_id != 4)
      F_ERROR("Expected block with id 4 (chars)",0);
   size_t chars_len = (read8(&info) - 48) * 1000 +
                      (read8(&info) - 48) * 100  +
                      (read8(&info) - 48) * 10   +
                      (read8(&info) - 48) * 1; //given values are in ascii...

}

void f_free(font_t* font)
{

}