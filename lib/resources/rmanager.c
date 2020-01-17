//
// Created by maxim on 1/1/20.
//

#ifdef __GNUC__
#pragma implementation "rmanager.h"
#endif

#include "rmanager.h"
#include "../structs.h"
#include "../graphics/material.h"
#include "texture.h"
#include "model.h"
#include "font.h"

#define RM_LOG(format, ...) DC_LOG("rmanager.c", format, __VA_ARGS__)
#define RM_ERROR(format, ...) DC_ERROR("rmanager.c", format, __VA_ARGS__)

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

struct _font_node {
   uint32_t id;
   font_t* font;
};

list_t*  textures;
list_t*  models;
list_t*  materials;
list_t*  fonts;

//
// rm_init()
//
void rm_init(void)
{
   textures = list_create(10);
   models = list_create(10);
   materials = list_create(10);
   fonts = list_create(10);
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
            if(((struct _texture_node*)textures->collection[i])->id == id) {
               return (((struct _texture_node*)textures->collection[i])->texture);
            }
         break;
      case MODEL:
         for(size_t i = 0; i < models->count; i++)
            if(((struct _model_node*)models->collection[i])->id == id) {
               return (((struct _model_node*)models->collection[i])->model);
            }
         break;
      case MATERIAL:
         for(size_t i = 0; i < materials->count; i++)
            if(((struct _material_node*)materials->collection[i])->id == id) {
               return (((struct _material_node*)materials->collection[i])->material);
            }
      case FONT:
         for(size_t i = 0; i < fonts->count; i++)
            if(((struct _font_node*)fonts->collection[i])->id == id) {
               return (((struct _font_node*)fonts->collection[i])->font);
            }
   }
   RM_ERROR("Unable to find %s with id \"%i\"",
            type == TEXTURE ? "texture" : (type == MODEL ? "model" : "material"),
            id);
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
            if(!strcmp(((struct _texture_node*)textures->collection[i])->texture->name, name)) {
               return (((struct _texture_node*)textures->collection[i])->texture);
            }
         break;
      case MODEL:
         for(size_t i = 0; i < models->count; i++)
            if(!strcmp(((struct _model_node*)models->collection[i])->model->filename, name)) {
               return (((struct _model_node*)models->collection[i])->model);
            }
         break;
      case MATERIAL:
         for(size_t i = 0; i < materials->count; i++)
            if(!strcmp(((struct _material_node*)materials->collection[i])->material->name, name)) {
               return (((struct _material_node*)materials->collection[i])->material);
            }
         break;
      case FONT:
         for(size_t i = 0; i < fonts->count; i++)
            if(!strcmp(((struct _font_node*)fonts->collection[i])->font->name, name)) {
               return (((struct _font_node*)fonts->collection[i])->font);
            }
         break;
   }

   RM_ERROR("Unable to find %s with name \"%s\"",
         type == TEXTURE ? "texture" : (type == MODEL ? "model" : "material"),
         name);
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
         struct _texture_node* node = malloc(sizeof(struct _texture_node));
         if(id == -1) node->id = textures->count;
         else node->id = id;
         node->texture = data;
         list_push(textures, node);
      }
         break;
      case MODEL:
      {
         struct _model_node* node = malloc(sizeof(struct _model_node));
         if(id == -1) node->id = models->count;
         else node->id = id;
         node->id = id;
         node->model = data;
         list_push(models, node);
      }
         break;
      case MATERIAL:
      {
         struct _material_node* node = malloc(sizeof(struct _material_node));
         if(id == -1) node->id = materials->count;
         else node->id = id;
         node->id = id;
         node->material = data;
         list_push(materials, node);
      }
         break;
      case FONT:
      {
         struct _font_node* node = malloc(sizeof(struct _font_node));
         if(id == -1) node->id = fonts->count;
         else node->id = id;
         node->id = id;
         node->font = data;
         list_push(fonts, node);
      }
         break;
   }
}

//
// rm_free()
//
void rm_free(bool free_tex, bool free_model, bool free_mat, bool free_font)
{
   if(free_tex) for(size_t i = 0; i < textures->count; i++)
         t_free(((struct _texture_node*)textures->collection[i])->texture);

   if(free_model) for(size_t i = 0; i < models->count; i++)
         m_free(((struct _model_node*)models->collection[i])->model);

   if(free_mat) for(size_t i = 0; i < materials->count; i++)
         mt_free(((struct _material_node*)materials->collection[i])->material);

   if(free_font) for(size_t i = 0; i < fonts->count; i++)
         f_free(((struct _font_node*)fonts->collection[i])->font);

   list_free_elements(textures);
   list_free_elements(models);
   list_free_elements(materials);
   list_free_elements(fonts);

   list_free(textures);
   list_free(models);
   list_free(materials);
   list_free(fonts);
}