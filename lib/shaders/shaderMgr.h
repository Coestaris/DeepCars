//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHADERMGR_H
#define DEEPCARS_SHADERMGR_H

#include <assert.h>

#include "shader.h"

#define SHM_MAX_SHADERS 256

#define SH_SIMPLECOLORED 0
#define SH_SIMPLECOLORED_COLOR 0
#define SH_SIMPLECOLORED_MODEL 1
#define SH_SIMPLECOLORED_VIEW  2
#define SH_SIMPLECOLORED_PROJ  3

typedef struct _shmNode
{
    shader_t* shader;
    int id;

} shmNode_t;

uint8_t s_hasShader(int id);
void s_push(shader_t* shader, int id);
void s_init(void);
void s_free(void);
shader_t* s_getShader(int id);

#endif //DEEPCARS_SHADERMGR_H
