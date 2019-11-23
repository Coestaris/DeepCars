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
texture_t* t_create(const char* fn)
{
   texture_t* t = malloc(sizeof(texture_t));
   t->fn = fn;
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
   if( tex->texID)
   {
      t_unload(tex);
   }

   free(tex->data);
   free(tex);
}

//
// t_unload()
//
void t_unload(texture_t* tex)
{
   printf("[texture.c]: Texture %s unloaded\n", tex->fn);
   GL_CALL(glDeleteTextures(1, &tex->texID));
   tex->texID = 0;
}

//
// t_load()
//
void t_load(texture_t* tex)
{
   tex->texID = oilTextureFromPngFile(
           (char*) tex->fn,
           GL_RGBA,
           OIL_TEX_WRAPPING | OIL_TEX_FLIPX | OIL_TEX_FLIPY |
           OIL_TEX_BORDERCOLOR | OIL_TEX_MIN | OIL_TEX_MAG,
           tex->data);

   if (!tex->texID)
   {
      oilPrintError();
      exit(EXIT_FAILURE);
   }

   tex->width = tex->data->out_width;
   tex->height = tex->data->out_height;

   printf("[texture.c]: Texture %s loaded. Width: %i, Height: %i\n",
          tex->fn, tex->width, tex->height);
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
