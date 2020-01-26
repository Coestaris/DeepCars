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
   // skip info block
   info += info_size;

   block_id = read8(&info);
   if(block_id != 2)
      F_ERROR("Expected block with id 2 (common)",0);
   size_t common_size = read32(&info);
   f->line_height = read16(&info) / FONT_SCALE;
   f->base = read16(&info) / FONT_SCALE;
   //skip scales
   info += 4;
   size_t pages = read16(&info);
   //skip other useless common data
   info += 5;
   if(pages != 1)
      F_ERROR("Using more than 1 page is not supported",0);

   block_id = read8(&info);
   if(block_id != 3)
      F_ERROR("Expected block with id 3 (pages)",0);
   //given values are in ascii...
   size_t pages_size = (read8(&info) - 48) * 10 + (read8(&info) - 48);
   //skip pages info
   info += pages_size;

   block_id = read8(&info);
   if(block_id != 4)
      F_ERROR("Expected block with id 4 (chars)",0);
   //given values are in ascii...
   size_t chars_len = (read8(&info) - 48) * 1000 +
                      (read8(&info) - 48) * 100  +
                      (read8(&info) - 48) * 10   +
                      (read8(&info) - 48) * 1;

   f->chars = chars_len / 20; //size of structure
   for(size_t i = 0; i < sizeof(f->infos) / sizeof(f->infos[0]); i++)
      f->infos[i].id = -1;

   for(size_t i = 0; i < f->chars; i++)
   {
      uint32_t id = read32(&info);
      if(id > 256)
         F_ERROR("Unsupported char",0);

      uint16_t x = read16(&info);
      uint16_t y = read16(&info);

      uint16_t widht = read16(&info);
      uint16_t height = read16(&info);

      int16_t xoffset = read16(&info);
      int16_t yoffset = read16(&info);

      uint16_t xadvance = read16(&info);

      f->infos[id].id = id;
      f->infos[id].width = (float)widht / FONT_SCALE;
      f->infos[id].height = (float)height / FONT_SCALE;
      f->infos[id].xoffset = (float)xoffset / FONT_SCALE;
      f->infos[id].yoffset = (float)yoffset / FONT_SCALE;
      f->infos[id].xadvance = (float)xadvance / FONT_SCALE;

      float x1 = (float)x / FONT_SCALE;
      float y1 = (float)y / FONT_SCALE;
      float x2 = (float)(x + widht) / FONT_SCALE;
      float y2 = (float)(y + height) / FONT_SCALE;
      f->infos[id].tex_coord[0] = x2 / (float)f->texture->width;  //top left
      f->infos[id].tex_coord[1] = y2 / (float)f->texture->height;

      f->infos[id].tex_coord[2] = x2 / (float)f->texture->width; // top right
      f->infos[id].tex_coord[3] = y1 / (float)f->texture->height;

      f->infos[id].tex_coord[4] = x1 / (float)f->texture->width; // bottom right
      f->infos[id].tex_coord[5] = y1 / (float)f->texture->height;

      f->infos[id].tex_coord[6] = x1 / (float)f->texture->width; // bottom left
      f->infos[id].tex_coord[7] = y2 / (float)f->texture->height;

      //skip page and channel
      info += 2;
   }

   GLuint indices[] = {
         0, 1, 3,
         1, 2, 3
   };

   GLuint ebo;
   GL_CALL(glGenVertexArrays(1, &f->vao));
   GL_CALL(glGenBuffers(1, &f->vbo));
   GL_CALL(glGenBuffers(1, &ebo));
   GL_CALL(glBindVertexArray(f->vao));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, f->vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 5, NULL, GL_DYNAMIC_DRAW));

   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
   GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

   GL_CALL(glEnableVertexAttribArray(0));
   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));

   GL_CALL(glEnableVertexAttribArray(1));
   GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

   return f;
}

void f_free(font_t* font)
{

}