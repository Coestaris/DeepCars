//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHM_H
#define DEEPCARS_SHM_H

#include <assert.h>

#include "shader.h"

// "SIMPLECOLORED" shader uniform indices
#define SH_SIMPLECOLORED 0
#define SH_SIMPLECOLORED_COLOR 0
#define SH_SIMPLECOLORED_MODEL 1
#define SH_SIMPLECOLORED_VIEW  2
#define SH_SIMPLECOLORED_PROJ  3
#define SH_SIMPLECOLORED_VIEWER  4

// Return true if shader with specified id stored in manager
bool s_hasShader(int id);

// Adds specified shader to a manager's list
void s_push(shader_t* shader, int id);

// Inits all necessary resources
void s_init(void);

// Frees all used by manager resources
void s_free(void);

// Returns shader with specified id
shader_t* s_getShader(int id);

#endif //DEEPCARS_SHM_H
