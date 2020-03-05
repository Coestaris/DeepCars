//
// Created by maxim on 1/5/20.
//

#ifdef __GNUC__
#pragma implementation "ssao.h"
#endif
#include "ssao.h"
#include "../../lib/resources/rmanager.h"

texture_t* generate_noise(size_t n)
{
   size_t size = n * n;

   float* floats = DEEPCARS_MALLOC(sizeof(float) * size * 3);

   for (size_t i = 0; i < size - 1; i++)
   {
      floats[i * 3 + 0] = (float)drand48() * 2.0f - 1.0f;
      floats[i * 3 + 1] = (float)drand48() * 2.0f - 1.0f;
      floats[i * 3 + 2] = 0;
   }

   GLuint noise_texture;
   GL_CALL(glGenTextures(1, &noise_texture));
   GL_CALL(glBindTexture(GL_TEXTURE_2D, noise_texture));
   GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, n, n, 0, GL_RGB, GL_FLOAT, floats));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
   GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
   GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

   texture_t* t = t_create(strdup("__generated_noise"));
   t->texID = noise_texture;
   t->mipmaps = 1;
   t->width = n;
   t->height = n;
   t->type = GL_TEXTURE_2D;
   rm_push(TEXTURE, t, -1);

   DEEPCARS_FREE(floats);

   return t;
}

float lerp(float a, float b, float f)
{
   return a + f * (b - a);
}

vec4* generate_kernel(size_t n)
{
   vec4* vectors = DEEPCARS_MALLOC(sizeof(vec4) * n);
   for (size_t i = 0; i < n; i++)
   {
      vec4 sample = cvec4(
            (float)drand48() * 2.0f - 1.0f,
            (float)drand48() * 2.0f - 1.0f,
            (float)drand48(),
            0);
      vec4_norm(sample);
      vec4_mulf(sample, (float)drand48());

      float scale = i / (float)n;

      scale = lerp(0.1f, 1.0f, scale * scale);
      vec4_mulf(sample, scale);
      vectors[i] = sample;
   }

   return vectors;
}

