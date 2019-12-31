//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHM_H
#define DEEPCARS_SHM_H

#include <assert.h>

#include "shader.h"

// Return true if shader with specified id stored in manager
bool s_has_shader(int id);

// Adds specified shader to a manager's list
void s_push(shader_t* shader, int id);

// Inits all necessary resources
void s_init(void);

// Frees all used by manager resources
void s_free(bool free_shaders);

// Returns shader with specified id
shader_t* s_get_shader(int id);

#endif //DEEPCARS_SHM_H
