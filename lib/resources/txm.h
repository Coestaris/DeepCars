//
// Created by maxim on 11/23/19.
//

#ifndef DEEPCARS_TXM_H
#define DEEPCARS_TXM_H

#include <stddef.h>

#include "texture.h"

// Inits all necessary txm resources
void txm_init();

// Frees all txm resources. if 'free_tex' set to 1 also frees al its textures
void txm_free(bool free_tex);

// Returns the texture with the specified identifier.
// If the texture with the specified identifier cannot be found, NULL is returned
texture_t* txm_get(size_t id);

// Adds new texture to texture manager
void txm_push(size_t id, texture_t* tex);

#endif //DEEPCARS_TXM_H
