//
// Created by maxim on 1/1/20.
//

#ifdef __GNUC__
#pragma implementation "rmanager.h"
#endif

#include "rmanager.h"
#include "../structs.h"
#include "texture.h"
#include "../graphics/material.h"
#include "model.h"

struct _texture_node {
   uint32_t id;
   texture_t* texture;
};

struct _model_node {
   uint32_t id;
   model_t* model;
};

struct _material_node {
   uint32_t id;
   material_t* material;
};

list_t*  textures;
list_t*  models;
list_t*  materials;

//
// rm_init()
//
void rm_init(void)
{
   textures = list_create(10);
   models = list_create(10);
   materials = list_create(10);
}

//
// rm_get()
//
void* rm_get(resource_type_t type, uint32_t id)
{
   switch(type)
   {
      case TEXTURE:
         for(size_t i = 0; i < textures->count; i++)
            if(((struct _texture_node*)textures->collection[0])->id == id) {
               return textures->collection[0];
            }
         break;
      case MODEL:
         for(size_t i = 0; i < textures->count; i++)
            if(((struct _model_node*)models->collection[0])->id == id) {
               return models->collection[0];
            }
         break;
      case MATERIAL:
         for(size_t i = 0; i < textures->count; i++)
            if(((struct _material_node*)materials->collection[0])->id == id) {
               return materials->collection[0];
            }
         break;
   }
   return NULL;
}

//
// rm_getn()
//
void* rm_getn(resource_type_t type, const char* name)
{
   switch(type)
   {
      case TEXTURE:
         for(size_t i = 0; i < textures->count; i++)
            if(!strcmp(((struct _texture_node*)textures->collection[0])->texture->name, name)) {
               return textures->collection[0];
            }
         break;
      case MODEL:
         for(size_t i = 0; i < textures->count; i++)
            if(!strcmp(((struct _model_node*)models->collection[0])->model->filename, name)) {
               return models->collection[0];
            }
         break;
      case MATERIAL:
         for(size_t i = 0; i < textures->count; i++)
            if(!strcmp(((struct _material_node*)materials->collection[0])->material->name, name)) {
               return materials->collection[0];
            }
         break;
   }
   return NULL;
}

//
// rm_push()
//
void rm_push(resource_type_t type, void* data, uint32_t id)
{
   switch(type)
   {
      case TEXTURE:
      {
         struct _texture_node* node = malloc(sizeof(struct _texture_node*));
         node->id = id;
         node->texture = data;
         list_push(textures, node);
      }
         break;
      case MODEL:
      {
         struct _model_node* node = malloc(sizeof(struct _model_node*));
         node->id = id;
         node->model = data;
         list_push(models, node);
      }
         break;
      case MATERIAL:
      {
         struct _material_node* node = malloc(sizeof(struct _material_node*));
         node->id = id;
         node->material = data;
         list_push(materials, node);
      }
         break;
   }
}

//
// rm_free()
//
void rm_free(bool free_tex, bool free_model, bool free_mat)
{
   if(free_tex) for(size_t i = 0; i < textures->count; i++)
         t_free(textures->collection[i]);

   if(free_model) for(size_t i = 0; i < models->count; i++)
         t_free(models->collection[i]);

   if(free_mat) for(size_t i = 0; i < materials->count; i++)
         t_free(materials->collection[i]);

   list_free_elements(textures);
   list_free_elements(models);
   list_free_elements(materials);

   list_free(textures);
   list_free(models);
   list_free(materials);
}


