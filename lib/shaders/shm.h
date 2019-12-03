//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_SHM_H
#define DEEPCARS_SHM_H

#include <assert.h>

#include "shader.h"

// "COLORED" shader uniform indices
#define SH_COLORED 0
#define SH_COLORED_COLOR 0
#define SH_COLORED_MODEL 1
#define SH_COLORED_VIEW  2
#define SH_COLORED_PROJ  3

// "COLORED_SHADED" shader uniform indices
#define SH_COLORED_SHADED 1
#define SH_COLORED_SHADED_COLOR 0
#define SH_COLORED_SHADED_MODEL 1
#define SH_COLORED_SHADED_VIEW  2
#define SH_COLORED_SHADED_PROJ  3
#define SH_COLORED_SHADED_VIEWER  4
#define SH_COLORED_SHADED_AMBIENT  5
#define SH_COLORED_SHADED_L_POS  6
#define SH_COLORED_SHADED_L_COLOR  7

// "TEXUTED" shader uniform indices
#define SH_TEXTURED 2
#define SH_TEXTURED_TEXTURE 0
#define SH_TEXTURED_MODEL 1
#define SH_TEXTURED_VIEW  2
#define SH_TEXTURED_PROJ  3
#define SH_TEXTURED_VIEWER  4

#define SH_BLUR 3
#define SH_BYPASS 4


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
