//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHM_H
#define DEEPCARS_SHM_H

#include <assert.h>

#include "shader.h"

// "COLORED" shader uniform indices
#define SH_DEFAULT 0
#define SH_BLUR 1
#define SH_BYPASS 2

// Return true if shader with specified id stored in manager
bool s_has_shader(int id);

// Adds specified shader to a manager's list
void s_push(shader_t* shader, int id);

// Inits all necessary resources
void s_init(void);

// Frees all used by manager resources
void s_free(bool free_shaders);

// Precalculates uniforms for default shaders
void s_setup_built_in_shaders();

// Returns shader with specified id
shader_t* s_get_shader(int id);

#endif //DEEPCARS_SHM_H
