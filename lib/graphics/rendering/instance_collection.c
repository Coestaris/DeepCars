//
// Created by maxim on 3/17/20.
//

#ifdef __GNUC__
#pragma implementation "instance_collection.h"
#endif
#include "instance_collection.h"

static list_t* instance_collections;

static void ic_load(instance_collection_t* ic)
{
   const size_t matsize = 16 * sizeof(float);
   mat4 buff = cmat4();

   float* buffer = DEEPCARS_MALLOC(ic->amount * matsize);
   for(size_t i = 0; i < ic->amount; i++)
   {
      mat4_print(ic->model_matrices[i]);
      mat4_trans(buff, ic->model_matrices[i]);
      mat4_print(buff);
      memcpy(buffer + i * 16, buff, matsize);
   }

   GLuint vbo;
   GL_CALL(glGenBuffers(1, &vbo));
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, ic->amount * matsize, buffer, GL_STATIC_DRAW));

   GL_CALL(glBindVertexArray(ic->model->VAO));
   // set attribute pointers for matrix (4 times vec4)
   GL_CALL(glEnableVertexAttribArray(3));
   GL_CALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, matsize, (void*)0));
   GL_CALL(glEnableVertexAttribArray(4));
   GL_CALL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, matsize, (void*)(sizeof(float) * 4)));
   GL_CALL(glEnableVertexAttribArray(5));
   GL_CALL(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, matsize, (void*)(2 * sizeof(float) * 4)));
   GL_CALL(glEnableVertexAttribArray(6));
   GL_CALL(glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, matsize, (void*)(3 * sizeof(float) * 4)));

   GL_CALL(glVertexAttribDivisor(3, 1));
   GL_CALL(glVertexAttribDivisor(4, 1));
   GL_CALL(glVertexAttribDivisor(5, 1));
   GL_CALL(glVertexAttribDivisor(6, 1));

   GL_CALL(glBindVertexArray(0));

   mat4_free(buff);
   DEEPCARS_FREE(buffer);
}

instance_collection_t* ic_create(model_t* model, material_t* mat, size_t amount)
{
   instance_collection_t* ic = DEEPCARS_MALLOC(sizeof(instance_collection_t));
   ic->model = model;
   ic->amount = amount;
   ic->material = mat;
   ic->model_matrices = DEEPCARS_MALLOC(sizeof(mat4) * amount);
   return ic;
}

void ic_free(instance_collection_t* ic, bool free_matrices)
{
   if(free_matrices)
      for(size_t i = 0; i < ic->amount; i++)
         mat4_free(ic->model_matrices[i]);

   DEEPCARS_FREE(ic->model_matrices);
   DEEPCARS_FREE(ic);
}

void ic_set_mat(instance_collection_t* ic, size_t index, mat4 mat)
{
   ic->model_matrices[index] = mat;
}

list_t* ic_get()
{
   return instance_collections;
}

void ic_push(instance_collection_t* ic)
{
   ic_load(ic);
   list_push(instance_collections, ic);
}

void ic_init(void)
{
   instance_collections = list_create();
}

void ic_release(bool free_collections, bool free_matrices)
{
   if(free_collections)
      for(size_t i = 0; i < instance_collections->count; i++)
         ic_free(instance_collections->collection[i], free_matrices);


   list_free(instance_collections);
}



