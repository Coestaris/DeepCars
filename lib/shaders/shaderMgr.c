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
    shader_t* simpleColored = sh_create("../lib/shaders/shaders/simpleColored.vsh",
                                        "../lib/shaders/shaders/simpleColored.fsh");
    sh_info(simpleColored);

    simpleColored->uniformLocations = malloc(sizeof(GLint) * 4);
    simpleColored->uniformLocations[SH_SIMPLECOLORED_COLOR] = glGetUniformLocation(simpleColored->progID, "objectColor");
    simpleColored->uniformLocations[SH_SIMPLECOLORED_MODEL] = glGetUniformLocation(simpleColored->progID, "model");
    simpleColored->uniformLocations[SH_SIMPLECOLORED_VIEW] = glGetUniformLocation(simpleColored->progID, "view");
    simpleColored->uniformLocations[SH_SIMPLECOLORED_PROJ] = glGetUniformLocation(simpleColored->progID, "projection");

    s_push(simpleColored, SH_SIMPLECOLORED);
}

void s_init()
{
    for (size_t i = 0; i < SHM_MAX_SHADERS; i++)
    {
        nodes[i] = malloc(sizeof(shmNode_t));
        nodes[i]->shader = NULL;
        nodes[i]->id = -1;
    }
    s_pushBuiltInShaders();
}

shader_t* s_getShader(int id)
{
    for (size_t i = 0; i < nodeCount; i++)
        if (nodes[i]->id == id) return nodes[i]->shader;
    return NULL;
}

void s_free(void)
{
    for (size_t i = 0; i < nodeCount; i++)
    {
        sh_free(nodes[i]->shader);
        nodes[i] = NULL;
    }
    nodeCount = 0;
}