//
// Created by maxim on 8/31/19.
//

#include "texture.h"

texture_t* t_create(const char* fn, vec2f_t center)
{
    texture_t* t = malloc(sizeof(texture_t));
    t->fn = fn;
    t->center = center;
    t->data = malloc(sizeof(texData));

    t->data->wrappingMode = OIL_DEFAULT_WRAPPING;
    t->data->flipX = OIL_DEFAULT_FLIPX;
    t->data->flipY = OIL_DEFAULT_FLIPY;
    t->data->borderColor = OIL_DEFAULT_BORDERCOLOR;
    t->data->minFilter = OIL_DEFAULT_MIN;
    t->data->magFilter = OIL_DEFAULT_MAG;

    return t;
}

void t_free(texture_t* tex)
{
    free(tex->data);
    free(tex);
}

void t_unload(texture_t* tex)
{
    glDeleteTextures(1, &tex->texID);
}

void t_load(texture_t* tex)
{
     tex->texID = oilTextureFromPngFile(
             (char*)tex->fn,
             GL_RGBA,
             OIL_TEX_WRAPPING | OIL_TEX_FLIPX | OIL_TEX_FLIPY |
                     OIL_TEX_BORDERCOLOR | OIL_TEX_MIN | OIL_TEX_MAG,
             tex->data);

     if(!tex->texID)
     {
         oilPrintError();
         abort();
     }

     tex->width = tex->data->out_width;
     tex->height = tex->data->out_height;

     printf("[texture.c]: Texture %s loaded. Width: %i, Height: %i\n",
             tex->fn, tex->width, tex->height);
}

inline void t_bind(texture_t* tex)
{
    glBindTexture(GL_TEXTURE_2D, tex->texID);
}
