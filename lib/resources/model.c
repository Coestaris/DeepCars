//
// Created by maxim on 9/11/19.
//

#include <errno.h>
#include "model.h"

objDescriptor_t allowedDescriptors[] =
{
    { "v",      od_vertex },
    { "vn",     od_vertexNormal },
    { "vt",     od_vertexTex },
    { "f",      od_face },
    { "g",      od_group },
    { "o",      od_object },
    { "mtllib", od_mtllib },
    { "usemtl", od_usemtl },
    { "#",      od_comment }
};

#define MAX_WORD_LEN 256
char word[MAX_WORD_LEN];
const size_t descriptorsCount = sizeof(allowedDescriptors) / sizeof(allowedDescriptors[0]);
vec4 buffVec;

bool isSeparator(char sym)
{
    return (sym == ' ' || sym == '\n' || sym == '\t' || sym == '\0');
}

bool readNextWord(const char* string, size_t* startIndex, size_t endIndex)
{
    if(*startIndex >= endIndex)
        return false;

    size_t start = *startIndex;

    while(isSeparator(string[start])) start++;
    *startIndex = start;

    while(!isSeparator(string[*startIndex])) (*startIndex)++;

    size_t len = *startIndex - start;
    assert(len < MAX_WORD_LEN);

    memset(word, 0, len + 1);
    memcpy(word, string + start, len);
    return true;
}

double parseDouble(size_t lineIndex)
{
    char* err;
    double d = strtod(word, &err);

    if(isSeparator(*err) && errno != ERANGE && errno != HUGE_VAL)
        return d;
    else
    {
        printf("Unable to parse %s. Expected float point value at line %li", word, lineIndex);
        exit(EXIT_FAILURE);
    }
}

void proceedLine(model_t* model, const char* string, size_t startIndex, size_t endIndex, size_t lineIndex)
{
    if(startIndex == endIndex)
        return;

    objDescriptor_t* descriptor = NULL;
    size_t wordCount = 0;

    while((readNextWord(string, &startIndex, endIndex)))
    {
        if(wordCount == 0)
        {
            for (size_t i = 0; i < descriptorsCount; i++)
                if (!strcmp(word, allowedDescriptors[i].string))
                {
                    descriptor = &allowedDescriptors[i];
                }

            if (descriptor == NULL)
            {
                printf("%s is unknown line descriptor at line %li", word, lineIndex);
                exit(EXIT_FAILURE);
            }

            printf("Descriptor: %s\n", descriptor->string);
        }
        else
        {
            if (descriptor->type == od_comment)
                continue;
            if(descriptor->type == od_vertex)
            {
                if(wordCount > 4) {
                    printf("Too many arguments for vertex descriptor at line %li", lineIndex);
                    exit(EXIT_FAILURE);
                }
                buffVec[wordCount - 1] = (float)parseDouble(lineIndex);
            }
            if(descriptor->type == od_vertexNormal)
            {
                if(wordCount > 3) {
                    printf("Too many arguments for normal descriptor at line %li", lineIndex);
                    exit(EXIT_FAILURE);
                }
                buffVec[wordCount - 1] = (float)parseDouble(lineIndex);
            }
        }
        wordCount++;
    }

    if (descriptor->type == od_comment)
        return;
    if (descriptor->type == od_vertex)     {
        m_pushVertex(model, vec4_ccpy(buffVec));
        fillVec4(buffVec, 0, 0, 0, 0);
    }
    if (descriptor->type == od_vertexNormal)     {
        m_pushNormal(model, vec4_ccpy(buffVec));
        fillVec4(buffVec, 0, 0, 0, 0);
    }
}

void getIndexes(model_t* model, const char* str)
{
    size_t startIndex = 0;
    size_t endIndex = 0;
    size_t lineIndex = 0;

    size_t i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != '\n' && str[i] != '\0')
        {
            startIndex = i;
            while(str[i] != '\n' && str[i] != '\0')
            {
                i++;
            }
            endIndex = i - 1;
            proceedLine(model, str, startIndex, endIndex, lineIndex++);
        }
        else
        {
            if(str[i] == '\n' && endIndex != i - 1)
                lineIndex++;
            i++;
        }
    }
}

model_t* m_create()
{
    model_t* model = malloc(sizeof(model_t));
    model->modelLen = malloc(sizeof(modelLen_t));
    model->modelLen->verticesMaxCount = START_LEN_COUNT;
    model->modelLen->normalsMaxCount = START_LEN_COUNT;
    model->modelLen->texCoordsMaxCount = START_LEN_COUNT;
    model->modelLen->facesMaxCount = START_LEN_COUNT;
    model->modelLen->groupNamesMaxCount = START_LEN_COUNT;
    model->modelLen->mtlLibsMaxCount = START_LEN_COUNT;
    model->modelLen->usedMaterialsMaxCount = START_LEN_COUNT;

    model->modelLen->verticesCount = 0;
    model->modelLen->normalsCount = 0;
    model->modelLen->texCoordsCount = 0;
    model->modelLen->facesCount = 0;
    model->modelLen->groupNamesCount = 0;
    model->modelLen->mtlLibsCount = 0;
    model->modelLen->usedMaterialsCount = 0;

    model->vertices = malloc(sizeof(vec4) * model->modelLen->verticesMaxCount);
    memset(model->vertices, 0, sizeof(vec4) * model->modelLen->verticesMaxCount);

    model->normals = malloc(sizeof(vec4) * model->modelLen->normalsMaxCount);
    memset(model->normals, 0, sizeof(vec4) * model->modelLen->normalsMaxCount);

    model->texCoords = malloc(sizeof(vec4) * model->modelLen->texCoordsMaxCount);
    memset(model->texCoords, 0, sizeof(vec4) * model->modelLen->texCoordsMaxCount);

    model->faces = malloc(sizeof(modelFace_t**) * model->modelLen->facesMaxCount);
    memset(model->faces, 0, sizeof(modelFace_t**) * model->modelLen->facesMaxCount);

    model->groupNames = malloc(sizeof(char*) * model->modelLen->groupNamesMaxCount);
    memset(model->groupNames, 0, sizeof(char*) * model->modelLen->groupNamesMaxCount);

    model->mtlLibs = malloc(sizeof(char*) * model->modelLen->mtlLibsMaxCount);
    memset(model->mtlLibs, 0, sizeof(char*) * model->modelLen->mtlLibsMaxCount);

    model->usedMaterials = malloc(sizeof(material_t*) * model->modelLen->usedMaterialsMaxCount);
    memset(model->usedMaterials, 0, sizeof(material_t*) * model->modelLen->usedMaterialsMaxCount);

    model->objectName = NULL;
    model->filename = NULL;

    return model;
}

model_t* m_load(const char* filename)
{
    if(!buffVec)
        buffVec = cvec4(0,0,0,0);

    FILE* f = fopen(filename, "r");
    if(!f)
    {
        printf("Unable to open file \"%s\"", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = malloc(size + 1);
    memset(data, 0, size + 1);

    fread(data, size, 1, f);

    model_t* model = m_create();
    model->filename = filename;

    getIndexes(model, data);

    free(data);
    fclose(f);
    return model;
}

void m_free(model_t* model)
{

}

inline void m_pushVertex(model_t* model, vec4 vec)
{
    assert(vec);
    pushModelProp(model, vertices, vec4, vec);
}

inline void m_pushNormal(model_t* model, vec4 vec)
{
    assert(vec);
    pushModelProp(model, normals, vec4, vec);
}

inline void m_pushTexCoord(model_t* model, vec4 vec)
{
    assert(vec);
    pushModelProp(model, texCoords, vec4, vec);
}

inline void m_pushFace(model_t* model, modelFace_t* face)
{
    assert(face);
    pushModelProp(model, faces, modelFace_t*, face);
}

inline void m_pushGroupName(model_t* model, char* groupName)
{
    assert(groupName);
    pushModelProp(model, groupNames, char*, groupName);
}

inline void m_pushMtlLib(model_t* model, char* mtlLib)
{
    assert(mtlLib);
    pushModelProp(model, mtlLibs, char*, mtlLib);
}

inline void m_pushUsedMaterial(model_t* model, material_t* usedMaterial)
{
    assert(usedMaterial);
    pushModelProp(model, usedMaterials, material_t*, usedMaterial);
}

void m_info(model_t* model)
{
    printf("Vertices (%li): \n", model->modelLen->verticesCount);
    for(size_t i = 0; i < model->modelLen->verticesCount; i++)
        printf("%li: %f %f %f [%f]\n",
                i, model->vertices[i][0],
                   model->vertices[i][1],
                   model->vertices[i][2],
                   model->vertices[i][3]);

    printf("\nNormals (%li): \n", model->modelLen->normalsCount);
    for(size_t i = 0; i < model->modelLen->normalsCount; i++)
        printf("%li: %f %f %f\n",
               i, model->normals[i][0],
                  model->normals[i][1],
                  model->normals[i][2]);
}