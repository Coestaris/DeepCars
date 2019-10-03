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

#define pushModelProp(model, prop, type, item) {                                                \
        if(model->modelLen->prop ## Count > model->modelLen->prop ##MaxCount - 1)               \
        {                                                                                       \
            size_t newLen = (int)((float)model->modelLen->prop ##MaxCount * INCREASE_LEN);      \
            model->prop = realloc(model-> prop, sizeof(type) * newLen);                         \
            model->modelLen->prop ##MaxCount = newLen;                                          \
        }                                                                                       \
        model->prop[model->modelLen->prop ##Count++] = item;                                    \
    }

#define START_LEN_COUNT 10
#define INCREASE_LEN 1.5
#define MAX_FACE_LEN 5

typedef struct _modelFace {
    struct _model* parent;

    uint16_t matID;
    uint16_t smID;
    uint32_t groupFlags;

    int32_t vertID[MAX_FACE_LEN];
    int32_t texID[MAX_FACE_LEN];
    int32_t normalID[MAX_FACE_LEN];

    size_t count;

} modelFace_t;

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
    size_t bufferLen;

    mat4 model;
    float* buffer;

} model_t;

typedef enum {
    od_vertex,
    od_vertexNormal,
    od_vertexTex,
    od_face,
    od_group,
    od_object,
    od_mtllib,
    od_usemtl,
    od_comment

} objDescriptorType_t;

typedef struct {
    const char* string;
    objDescriptorType_t type;

} objDescriptor_t;

model_t* m_create();
model_t* m_load(const char* filename);
void m_free(model_t* model);
void m_info(model_t* model);
void m_build(model_t* model);

void m_pushVertex(model_t* model, vec4 vec);
void m_pushNormal(model_t* model, vec4 vec);
void m_pushTexCoord(model_t* model, vec4 vec);
void m_pushFace(model_t* model, modelFace_t* face);
void m_pushGroupName(model_t* model, char* groupName);
void m_pushMtlLib(model_t* model, char* mtlLib);
void m_pushUsedMaterial(model_t* model, material_t* usedMaterial);

#endif //ZOMBOID3D_MODEL_H
