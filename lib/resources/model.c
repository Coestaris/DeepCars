//
// Created by maxim on 9/11/19.
//

#include "model.h"

void push_vertex()
{

}

void push_normal()
{

}

void push_face()
{

}

void push_texture()
{

}

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

} descriptor_t;

descriptor_t allowedDescriptors[] =
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

bool isSeparator(char sym)
{
    return (sym == ' ' || sym == '\n' || sym == '\t' || sym == '\0');
}

#define MAX_WORD_LEN 256
char word[MAX_WORD_LEN];
const size_t descriptorsCount = sizeof(allowedDescriptors) / sizeof(allowedDescriptors[0]);



bool readNextWord(char* string, size_t* startIndex, size_t endIndex)
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

void proceedLine(char* string, size_t startIndex, size_t endIndex)
{
    bool firstWord = true;
    descriptor_t* descriptor = NULL;

    while((readNextWord(string, &startIndex, endIndex)))
    {
        printf("Word %s\n", word);
        if(firstWord)
        {
            for (size_t i = 0; i < descriptorsCount; i++)
                if (!strcmp(word, allowedDescriptors[i].string))
                {
                    descriptor = &allowedDescriptors[i];
                }

            if (descriptor == NULL)
            {
                printf("%s is unknown line descriptor", word);
                exit(EXIT_FAILURE);
            }

            firstWord = false;
            printf("Descriptor: %s", descriptor->string);
        }
        else
        {
            if (descriptor->type == od_comment)
                continue;
            //if(descriptor->type == od_vertex)

        }
    }
}

void getIndexes(char* str)
{
    size_t startIndex = 0;
    size_t endIndex = 0;

    size_t i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != '\n' && str[i] != '\0')
        {
            startIndex = i;
            while(str[i] != '\n' && str[i] != '\0')
                i++;
            endIndex = i - 1;
            proceedLine(str, startIndex, endIndex);
        }
        else i++;
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

    return model;
}

model_t* m_load(const char* filename)
{
    //sizeof(allowedDescriptors) / sizeof(const char*)
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
    getIndexes(data);

    free(data);
    fclose(f);
    return model;
}

void m_free(model_t* model)
{

}