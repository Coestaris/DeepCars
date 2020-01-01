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
   model_t* texture;
};

struct _material_node {
   uint32_t id;
   material_t* texture;
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
         break;
      case MODEL:
         break;
      case MATERIAL:
         break;
   }
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
            if((struct))
         break;
      case MODEL:
         break;
      case MATERIAL:
         break;
   }
}

//
// rm_push()
//
void rm_push(resource_type_t type, void* data, uint32_t id)
{
   switch(type)
   {
      case TEXTURE:
         break;
      case MODEL:
         break;
      case MATERIAL:
         break;
   }
}

//
// rm_free()
//
void rm_free(bool free_tex, bool free_model, bool free_mat)
{

}


