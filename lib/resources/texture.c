//
// Created by maxim on 8/31/19.
//

#ifdef __GNUC__
#pragma implementation "texture.h"
#endif
#include "texture.h"

//
// t_create()
//
texture_t* t_create(char* name)
{
   texture_t* t = malloc(sizeof(texture_t));
   t->name = name;
   t->data = malloc(sizeof(texData));
   t->texID = 0;

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
   printf("[texture.c]: Texture \"%s\" unloaded\n", tex->name);
   if(tex->texID)
      GL_CALL(glDeleteTextures(1, &tex->texID));

   free(tex->name);
   free(tex->data);
   free(tex);
}

GLuint t_load_dds(uint8_t* source, texData* tex_data, char* dds_format)
{
   const char* str_format = NULL;

   uint32_t width = 0;
   uint32_t height = 0;
   uint32_t mip_map_count = 0;
   uint32_t block_size = 0;
   uint32_t format = 0;
   uint32_t w = 0;
   uint32_t h = 0;

   GLuint tid = 0;

   // compare the `DDS ` signature
   if(memcmp(source, "DDS ", 4) != 0)
      goto exit;

   // extract height, width, and amount of mipmaps - yes it is stored height then width
   height        = (source[12]) | (source[13] << 8U) | (source[14] << 16U) | (source[15] << 24U);
   width         = (source[16]) | (source[17] << 8U) | (source[18] << 16U) | (source[19] << 24U);
   mip_map_count = (source[28]) | (source[29] << 8U) | (source[30] << 16U) | (source[31] << 24U);

   tex_data->out_width = width;
   tex_data->out_height = height;

   // figure out what format to use for what fourCC file type it is
   // block size is about physical chunk storage of compressed data in file (important)
   if(source[84] == 'D')
   {
      switch(source[87])
      {
         case '1': // DXT1
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            block_size = 8;
            str_format = "DXT1";
            break;

         case '3': // DXT3
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            block_size = 16;
            str_format = "DXT3";
            break;

         case '5': // DXT5
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            block_size = 16;
            str_format = "DXT5";
            break;

         case '0':
            // DX10
            // unsupported, else will error
            // as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
            // so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
         default:
            str_format = "unknown";
            goto exit;
      }
   }
   else
   {
      str_format = "unknown";
      goto exit;
   }


   // prepare new incomplete texture
   GL_CALL(glGenTextures(1, &tid));
   if(tid == 0)
      goto exit;

   // bind the texture
   // make it complete by specifying all needed parameters and ensuring all mipmaps are filled
   GL_CALL(glBindTexture(GL_TEXTURE_2D, tid));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1)); // opengl likes array length of mipmaps
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_data->magFilter));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_data->minFilter)); // don't forget to enable mipmaping
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_data->wrappingMode));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_data->wrappingMode));

   // prepare some variables
   size_t offset = 0;
   size_t size = 0;
   w = width;
   h = height;

   // loop through sending block at a time with the magic formula
   // upload to OpenGL properly, note the offset transverses the pointer
   // assumes each mipmap is 1/2 the size of the previous mipmap
   for (size_t i=0; i < mip_map_count; i++)
   {
      if(w == 0 || h == 0)
      {
         // discard any odd mipmaps 0x1 0x2 resolutions
         mip_map_count--;
         continue;
      }

      size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
      GL_CALL(glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, source + 128 + offset));
      offset += size;
      w /= 2;
      h /= 2;
   }

   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1));
   GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

   exit:
   snprintf(dds_format, 100, "%s, Mipmaps: %i", str_format, mip_map_count);
   return tid;
}

//
// t_load_s()
//
void t_load_s(texture_t* tex, uint8_t* source, size_t length, bool png)
{
   char dds_format[100];

   if(png)
   {
      //todo:...
    /*  tex->texID = oilTextureFromPngFile(
              (char*)path,
              GL_RGBA,
              OIL_TEX_WRAPPING | OIL_TEX_FLIPX | OIL_TEX_FLIPY |
              OIL_TEX_BORDERCOLOR | OIL_TEX_MIN | OIL_TEX_MAG,
              tex->data);*/


   }
   else
   {
      tex->texID = t_load_dds(source, tex->data, (char*)dds_format);
   }

   tex->width = tex->data->out_width;
   tex->height = tex->data->out_height;

   if (!tex->texID)
   {
      oilPrintError();
      exit(EXIT_FAILURE);
   }


   if(!png)
   {
      printf("[texture.c]: DDS (%s) texture \"%s\" loaded. Width: %i, Height: %i\n",
             dds_format, tex->name, tex->width, tex->height);
   }
   else
   {
      printf("[texture.c]: PNG texture \"%s\" loaded. Width: %i, Height: %i\n",
              tex->name, tex->width, tex->height);
   }
}

//
// t_load()
//
void t_load(texture_t* tex, const char* path, bool png)
{
   FILE* f = fopen(path, "rb");
   if(!f)
   {
      printf("[texture.c][ERROR]: Unable to open file %s", path);
      exit(EXIT_FAILURE);
   }

   fseek(f, 0, SEEK_END);
   size_t len = ftell(f);
   fseek(f, 0, SEEK_SET);

   uint8_t* buffer = malloc(len + 1);
   if(fread(buffer, len, 1, f) != 1)
   {
      printf("[texture.c][ERROR]: Unable to read data from file");
      exit(EXIT_FAILURE);
   }

   t_load_s(tex, buffer, len, png);

   free(buffer);
   fclose(f);
}

//
// t_bind()
//
inline void t_bind(texture_t* tex, GLenum target)
{
   GL_PCALL(glActiveTexture(GL_TEXTURE0));
   if(!tex)
   {
      GL_PCALL(glBindTexture(GL_TEXTURE_2D, 0));
   }
   else
   {
      GL_PCALL(glBindTexture(GL_TEXTURE_2D, tex->texID));
   }
}
