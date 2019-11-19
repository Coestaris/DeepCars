//
// Created by maxim on 9/11/19.
//

#include <errno.h>
#include "model.h"


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
#define MAX_WORD_LEN 256

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

void m_pushVertex(model_t* model, vec4 vec);
void m_pushNormal(model_t* model, vec4 vec);
void m_pushTexCoord(model_t* model, vec4 vec);
void m_pushFace(model_t* model, modelFace_t* face);
void m_pushGroupName(model_t* model, char* groupName);
void m_pushMtlLib(model_t* model, char* mtlLib);
void m_pushUsedMaterial(model_t* model, material_t* usedMaterial);

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

modelFace_t* face;
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

void parseFace(int32_t* i1, int32_t* i2, int32_t* i3)
{
    int32_t* ptr = i1;
    size_t power = 0;
    for(int64_t i = strlen(word) - 1; i >= 0; i--)
    {
        if(word[i] == '/')
        {
            if(ptr == i1) ptr = i2;
            else ptr = i3;
            power = 0;
        }
        else
        {
            if(power == 0) *ptr = 0;
            *ptr += (word[i] - '0') * pow(10, power++);
        }
    }
}

double parseDouble(size_t lineIndex)
{
    char* err;
    double d = strtod(word, &err);

    if(isSeparator(*err) && errno != ERANGE && errno != HUGE_VAL)
        return d;
    else
    {
        printf("[model.c][ERROR]: Unable to parse %s. Expected float point value at line %li\n", word, lineIndex);
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
                printf("[model.c][ERROR]: %s is unknown line descriptor at line %li\n", word, lineIndex);
                exit(EXIT_FAILURE);
            }

            //printf("Descriptor: %s\n", descriptor->string);
        }
        else
        {
            if (descriptor->type == od_comment)
                continue;
            if(descriptor->type == od_vertex)
            {
                if(wordCount > 4) {
                    printf("[model.c][ERROR]: Too many arguments for vertex descriptor at line %li\n", lineIndex);
                    exit(EXIT_FAILURE);
                }
                buffVec[wordCount - 1] = (float)parseDouble(lineIndex);
            }
            if(descriptor->type == od_vertexNormal)
            {
                if(wordCount > 3) {
                    printf("[model.c][ERROR]: Too many arguments for normal descriptor at line %li\n", lineIndex);
                    exit(EXIT_FAILURE);
                }
                buffVec[wordCount - 1] = (float)parseDouble(lineIndex);
            }
            if(descriptor->type == od_face)
            {
                assert(wordCount < MAX_FACE_LEN);

                int32_t i1 = -1, i2 = -1, i3 = -1;
                parseFace(&i1, &i2, &i3);

                if(i2 == -1 && i3 == -1)
                {
                    face->vertID[wordCount - 1] = i1;
                }
                else
                {
                    face->vertID[wordCount - 1] = i3;
                    face->texID[wordCount - 1] = i2;
                    face->normalID[wordCount - 1] = i1;
                }
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

    if(descriptor->type == od_face) {
        modelFace_t* newFace = malloc(sizeof(modelFace_t));
        newFace->parent = model;
        newFace->count = wordCount - 1;
        memcpy(newFace->normalID, face->normalID, sizeof(face->normalID));
        memcpy(newFace->vertID, face->vertID, sizeof(face->vertID));
        memcpy(newFace->texID, face->texID, sizeof(face->texID));
        m_pushFace(model, newFace);

        memset(face->normalID, -1, sizeof(face->normalID));
        memset(face->texID, -1, sizeof(face->texID));
    }
}

void parseLines(model_t* model, const char* str)
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

    if(!face)
    {
        face = malloc(sizeof(modelFace_t));
        memset(face->normalID, -1, sizeof(face->normalID));
        memset(face->texID, -1, sizeof(face->texID));
    }
    FILE* f = fopen(filename, "r");
    if(!f)
    {
        printf("[model.c][ERROR]: Unable to open file \"%s\"", filename);
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

    parseLines(model, data);

    free(data);
    fclose(f);

    printf("[model.c]: Loaded model %s. Vertices: %li, Faces: %li\n",
            filename, model->modelLen->verticesCount, model->modelLen->facesCount);

    return model;
}

void m_free(model_t* model)
{
    for(size_t i = 0; i < model->modelLen->verticesCount; i++)
        freeVec4(model->vertices[i]);
    free(model->vertices);

    for(size_t i = 0; i < model->modelLen->normalsCount; i++)
        freeVec4(model->normals[i]);
    free(model->normals);

    for(size_t i = 0; i < model->modelLen->texCoordsCount; i++)
        freeVec4(model->texCoords[i]);
    free(model->texCoords);

    for(size_t i = 0; i < model->modelLen->facesCount; i++)
        free(model->faces[i]);
    free(model->faces);

    for(size_t i = 0; i < model->modelLen->groupNamesCount; i++)
        free(model->groupNames[i]);
    free(model->groupNames);

    for(size_t i = 0; i < model->modelLen->mtlLibsCount; i++)
        free(model->mtlLibs[i]);
    free(model->mtlLibs);

    for(size_t i = 0; i < model->modelLen->usedMaterialsCount; i++)
        free(model->usedMaterials[i]);
    free(model->usedMaterials);

    free(model->objectName);
    free(model->modelLen);

    if(model->VBO != 0)
        glDeleteBuffers(1, &(model->VBO));

    if(model->VAO != 0)
        glDeleteBuffers(1, &(model->VAO));

    printf("[model.c]: Freed model %s", model->filename);
    free(model);
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
    printf("[model.c]: Vertices (%li): \n", model->modelLen->verticesCount);
    for(size_t i = 0; i < model->modelLen->verticesCount; i++)
        printf("%li: %f %f %f [%f]\n",
                i, model->vertices[i][0],
                   model->vertices[i][1],
                   model->vertices[i][2],
                   model->vertices[i][3]);

    printf("\n[model.c]: Normals (%li): \n", model->modelLen->normalsCount);
    for(size_t i = 0; i < model->modelLen->normalsCount; i++)
        printf("%li: %f %f %f\n",
               i, model->normals[i][0],
                  model->normals[i][1],
                  model->normals[i][2]);

    printf("\n[model.c]: Faces (%li): \n", model->modelLen->facesCount);
    for(size_t i = 0; i < model->modelLen->facesCount; i++)
    {
        printf("%li. ", i);
        for(size_t j = 0; j < model->faces[j]->count; j++) {
            printf("%i/%i/%i ",
                    model->faces[i]->vertID[j],
                    model->faces[i]->texID[j],
                    model->faces[i]->normalID[j]);
        }
        putchar('\n');
    }
}

void m_build(model_t* model)
{
    bool useTexCoords = model->modelLen->texCoordsCount != 0 && model->faces[0]->texID[0] != -1;
    bool useNormals = model->modelLen->normalsCount != 0 && model->faces[0]->normalID[0] != -1;
    size_t vertPerFace =  3; //always triangles...

    size_t size =
                            model->modelLen->facesCount * vertPerFace * 3 +         //vertices
           (useTexCoords ? (model->modelLen->facesCount * vertPerFace * 2) : 0) +  //texcoord
           (useNormals ?   (model->modelLen->facesCount * vertPerFace * 3) : 0);   //normals

    float* buffer = malloc(sizeof(float) * size);
    size_t bufferIndex = 0;

    for(size_t i = 0; i < model->modelLen->facesCount; i++)
        for(size_t j = 0; j < vertPerFace; j++)
        {
            assert(model->faces[i]->vertID[j] <= model->modelLen->verticesCount);
            uint32_t vertID = model->faces[i]->vertID[j] - 1;

            buffer[bufferIndex++] = model->vertices[vertID][0];
            buffer[bufferIndex++] = model->vertices[vertID][1];
            buffer[bufferIndex++] = model->vertices[vertID][2];

            if(useTexCoords)
            {
                assert(model->faces[i]->texID[j] <= model->modelLen->texCoordsCount);
                uint32_t texID = model->faces[i]->texID[j] - 1;

                buffer[bufferIndex++] = model->texCoords[texID][0];
                buffer[bufferIndex++] = model->texCoords[texID][1];
            }

            if(useNormals)
            {
                assert(model->faces[i]->normalID[j] <= model->modelLen->normalsCount);
                uint32_t normalID = model->faces[i]->normalID[j] - 1;

                buffer[bufferIndex++] = model->normals[normalID][0];
                buffer[bufferIndex++] = model->normals[normalID][1];
                buffer[bufferIndex++] = model->normals[normalID][2];
            }
        }

    assert(bufferIndex != size - 1);

    glGenBuffers(1, &model->VBO);
    glGenVertexArrays(1, &model->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, buffer, GL_STATIC_DRAW);

    glBindVertexArray(model->VAO);
    if(useNormals && useTexCoords)
    {
        glVertexAttribPointer(0, vertPerFace, GL_FLOAT, GL_FALSE, (vertPerFace + 5) * sizeof(float),
                (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (vertPerFace + 5) * sizeof(float),
                (void*)(vertPerFace * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (vertPerFace + 5) * sizeof(float),
                (void*)((vertPerFace + 2) * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    else if(!useNormals && useTexCoords)
    {
        glVertexAttribPointer(0, vertPerFace, GL_FLOAT, GL_FALSE, (vertPerFace + 2) * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (vertPerFace + 2) * sizeof(float),
                              (void*)(vertPerFace * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else if(useNormals && !useTexCoords)
    {
        glVertexAttribPointer(0, vertPerFace, GL_FLOAT, GL_FALSE, (vertPerFace + 3) * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (vertPerFace + 3) * sizeof(float),
                              (void*)(vertPerFace * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else //no normals and coords
    {
        glVertexAttribPointer(0, vertPerFace, GL_FLOAT, GL_FALSE, vertPerFace * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(model->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*for(size_t i = 0; i < size; i++)
        printf("%.2f%c", buffer[i], i && !((i + 1) % 6) ? '\n' : ' ');*/

    free(buffer);
}