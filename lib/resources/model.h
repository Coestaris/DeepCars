//
// Created by maxim on 9/11/19.
//

#ifndef ZOMBOID3D_MODEL_H
#define ZOMBOID3D_MODEL_H

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

struct _model;

typedef struct _modelFace {
    struct _model* parent;

    uint16_t matID;
    uint16_t smID;
    uint32_t groupFlags;

    size_t* vertID;
    size_t* texID;
    size_t* normalID;

} modelFace_t;

#define START_LEN_COUNT 10
#define INCREASE_LEN 1.5

typedef struct _modelLen {
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

typedef struct _model {
    vec4* vertices;
    vec4* normals;
    vec4* texCoords;

    modelFace_t** faces;

    char* objectName;
    char** groupNames;

    char** mtlLibs;
    material_t** usedMaterials;

    modelLen_t* modelLen;

} model_t;

model_t* m_create();
model_t* m_load(const char* filename);
void m_free(model_t* model);

#endif //ZOMBOID3D_MODEL_H
