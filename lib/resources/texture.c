//
// Created by maxim on 8/31/19.
//

#ifdef __GNUC__
#pragma implementation "texture.h"
#endif
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define T_LOG(format, ...) DC_LOG("texture.c", format, __VA_ARGS__)
#define T_ERROR(format, ...) DC_ERROR("texture.c", format, __VA_ARGS__)

//
// t_create()
//
texture_t* t_create(char* name)
{
   assert(name);

   texture_t* t = DEEPCARS_MALLOC(sizeof(texture_t));
   t->name = name;
   t->data = DEEPCARS_MALLOC(sizeof(texData));
   t->texID = 0;
   t->mipmaps = 0;

   t->data->wrappingMode = OIL_DEFAULT_WRAPPING;
   t->data->flipX = OIL_DEFAULT_FLIPX;
   t->data->flipY = true;
   t->data->borderColor = OIL_DEFAULT_BORDERCOLOR;
   t->data->minFilter = OIL_DEFAULT_MIN;
   t->data->magFilter = OIL_DEFAULT_MAG;

   return t;
}

//
// t_free()
//
void t_free(texture_t* tex)
{
   assert(tex);

   T_LOG("Texture \"%s\" unloaded", tex->name);
   if(tex->texID)
   GL_CALL(glDeleteTextures(1, &tex->texID));

   DEEPCARS_FREE(tex->name);
   DEEPCARS_FREE(tex->data);
   DEEPCARS_FREE(tex);
}

//
// t_set_params()
//
void t_set_params(texture_t* texture, GLenum target, uint32_t width, uint32_t height)
{
   assert(texture);
   assert(target);
   assert(width && height);

   GL_CALL(glGenTextures(1, &texture->texID));
   if(!texture->texID) T_ERROR("Unable to create OpenGL texture",0);

   GL_CALL(glBindTexture(target, texture->texID));

   GL_CALL(glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0));
   GL_CALL(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texture->data->magFilter));
   GL_CALL(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texture->data->minFilter)); // don't forget to enable mipmaping
   GL_CALL(glTexParameteri(target, GL_TEXTURE_WRAP_S, texture->data->wrappingMode));
   GL_CALL(glTexParameteri(target, GL_TEXTURE_WRAP_T, texture->data->wrappingMode));

   GL_CALL(glBindTexture(target, 0));

   texture->type = target;
   texture->width = width;
   texture->height = height;
}

//
// t_get_pretty_signature()
//
char* t_get_pretty_signature(texture_t* t, const char* str_descr)
{
   assert(t);
   assert(str_descr);

   char* buffer = DEEPCARS_MALLOC(sizeof(char) * 100);

   snprintf(buffer, 100, "%s %s \"%s\" [%ix%i] (%li mipmap%s%s)",
         str_descr,
         t->type == GL_TEXTURE_2D ? "texture" : "cubemap",
         t->name, t->width, t->height,
         t->mipmaps,
         t->mipmaps > 1 ? "s" : "",
         t->type == GL_TEXTURE_2D ? "" : " per face");

   return buffer;

}

//
// t_set_data_png()
//
void t_set_data_png(char const** str_descr, texture_t* texture, GLenum fill_target, uint8_t* source, size_t length)
{
   assert(str_descr);
   assert(texture);
   assert(source);
   assert(length);

   int32_t width, height, channels;
   uint8_t *img = stbi_load_from_memory(source, length, &width, &height, &channels, STBI_rgb_alpha);
   if(img == NULL)
   {
      T_ERROR("Error in loading PNG the image\n",0);
   }

   assert(width == texture->width);
   assert(height == texture->height);

   GL_PCALL(glBindTexture(texture->type, texture->texID));
   GL_PCALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
   if(channels == 3)
      {GL_PCALL(glTexImage2D(fill_target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img));}
   else if(channels == 4)
      {GL_PCALL(glTexImage2D(fill_target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img))};

   GL_CALL(glTexParameteri(texture->type, GL_TEXTURE_MAX_LEVEL, 0));
   GL_PCALL(glBindTexture(texture->type, 0));

   DEEPCARS_FREE(img);
   *str_descr = "PNG";
}

//
// t_set_data_dds()
//
void t_set_data_dds(char const** str_descr, texture_t* texture, GLenum fill_target, uint8_t* source, size_t length)
{
   assert(str_descr);
   assert(texture);
   assert(source);
   assert(length);

   *str_descr = NULL;

   uint32_t width = 0;
   uint32_t height = 0;
   uint32_t mip_map_count = 0;
   uint32_t block_size = 0;
   uint32_t format = 0;
   uint32_t w = 0;
   uint32_t h = 0;

   GLuint tid = 0;

   // compare the `DDS ` signature
   if (memcmp(source, "DDS ", 4) != 0)
   {
      T_ERROR("DDS signatures doesn't match",0)
   }

   // extract height, width, and amount of mipmaps - yes it is stored height then width
   height        = (source[12]) | (source[13] << 8U) | (source[14] << 16U) | (source[15] << 24U);
   width         = (source[16]) | (source[17] << 8U) | (source[18] << 16U) | (source[19] << 24U);
   mip_map_count = (source[28]) | (source[29] << 8U) | (source[30] << 16U) | (source[31] << 24U);

   if(source[84] == 'D')
   {
      switch(source[87])
      {
         case '1': // DXT1
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            block_size = 8;
            *str_descr = "DXT1";
            break;

         case '3': // DXT3
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            block_size = 16;
            *str_descr = "DXT3";
            break;

         case '5': // DXT5
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            block_size = 16;
            *str_descr = "DXT5";
            break;

         case '0':
            // DX10
            // unsupported, else will error
            // as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
            // so, buffer = DEEPCARS_MALLOC((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
         default:
            *str_descr = "unknown";
            goto exit;
      }
   }
   else
   {
      *str_descr = "unknown";
      goto exit;
   }

   GL_CALL(glBindTexture(texture->type, texture->texID));

   // prepare some variables
   size_t offset = 0;
   size_t size = 0;
   w = width;
   h = height;
   texture->mipmaps = mip_map_count;

   for (size_t i=0; i < mip_map_count; i++)
   {
      if(w == 0 || h == 0)
      {
         // discard any odd mipmaps 0x1 0x2 resolutions
         mip_map_count--;
         continue;
      }

      size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
      GL_CALL(glCompressedTexImage2D(fill_target, i, format, w, h, 0, size, source + 128 + offset));
      offset += size;
      w /= 2;
      h /= 2;
   }

   GL_CALL(glTexParameteri(texture->type, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1));
   GL_CALL(glBindTexture(texture->type, 0));

   exit:
   return;
}


//
// t_bind()
//
inline void t_bind(texture_t* tex, GLenum active)
{
   GL_PCALL(glActiveTexture(GL_TEXTURE0 + active));
   if(!tex)
   {
      GL_PCALL(glBindTexture(GL_TEXTURE_2D, 0));
   }
   else
   {
      GL_PCALL(glBindTexture(tex->type, tex->texID));
   }
}
