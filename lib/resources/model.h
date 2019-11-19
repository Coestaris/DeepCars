//
// Created by maxim on 9/11/19.
//

#ifndef DEEPCARS_MODEL_H
#define DEEPCARS_MODEL_H

#define GL_GLEXT_PROTOTYPES

#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include "GL/gl.h"
#include "../graphics/gmath.h"
#include "../graphics/material.h"

#define MAX_FACE_LEN 5

struct _model;

typedef struct _modelFace
{
   struct _model* parent;

   uint16_t matID;
   uint16_t smID;
   uint32_t groupFlags;

   int32_t vertID[MAX_FACE_LEN];
   int32_t texID[MAX_FACE_LEN];
   int32_t normalID[MAX_FACE_LEN];

   size_t count;

} modelFace_t;

typedef struct _modelLen
{
   size_t verticesCount;
   size_t normalsCount;
   size_t texCoordsCount;
   size_t facesCount;
   size_t groupNamesCount;
   size_t mtlLibsCount;
   size_t usedMaterialsCount;
   size_t verticesMaxCount;
   size_t normalsMaxCount;
   size_t texCoordsMaxCount;
   size_t facesMaxCount;
   size_t groupNamesMaxCount;
   size_t mtlLibsMaxCount;
   size_t usedMaterialsMaxCount;

} modelLen_t;

typedef struct _model
{
   const char* filename;

   vec4* vertices;
   vec4* normals;
   vec4* texCoords;

   modelFace_t** faces;

   char* objectName;
   char** groupNames;

   char** mtlLibs;
   material_t** usedMaterials;

   modelLen_t* modelLen;

   GLuint VBO;
   GLuint VAO;

} model_t;

model_t* m_create();
model_t* m_load(const char* filename);
void m_free(model_t* model);
void m_info(model_t* model);
void m_build(model_t* model);

#endif //DEEPCARS_MODEL_H
