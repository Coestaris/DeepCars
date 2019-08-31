//
// Created by maxim on 8/31/19.
//

#ifndef ZOMBOID3D_SHADERMGR_H
#define ZOMBOID3D_SHADERMGR_H

#include <assert.h>

#include "shader.h"

#define SHADER_IMAGE 0
#define SHADER_BACKGROUND 1
#define SHADER_TEXT 2

#define SHM_MAX_SHADERS 256

typedef struct _shmNode
{
    shader_t* shader;
    int id;
} shmNode_t;

uint8_t s_hasShader(int id);
void s_pushBuiltInShaders();
void s_push(shader_t* shader, int id);
void s_init();
shader_t* s_getShader(int id);

#endif //ZOMBOID3D_SHADERMGR_H
