//
// Created by maxim on 4/29/19.
//

#include "shaderMgr.h"

shmNode_t* nodes[SHM_MAX_SHADERS];
size_t nodeCount;

uint8_t s_hasShader(int id)
{
    for (size_t i = 0; i < nodeCount; i++)
        if (nodes[i]->id == id) return 1;
    return 0;
}

void s_push(shader_t* shader, int id)
{
    assert(shader != NULL);
    assert(!s_hasShader(id));

    nodes[nodeCount]->shader = shader;
    nodes[nodeCount++]->id = id;
}

void s_pushBuiltInShaders()
{
}

void s_init()
{
    for (size_t i = 0; i < SHM_MAX_SHADERS; i++)
    {
        nodes[i] = malloc(sizeof(shmNode_t));
        nodes[i]->shader = NULL;
        nodes[i]->id = -1;
    }
}

shader_t* s_getShader(int id)
{
    for (size_t i = 0; i < nodeCount; i++)
        if (nodes[i]->id == id) return nodes[i]->shader;
    return NULL;
}