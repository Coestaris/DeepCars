//
// Created by maxim on 11/23/19.
//

#ifndef DEEPCARS_TXM_H
#define DEEPCARS_TXM_H

#include <stddef.h>

#include "texture.h"

// Maximum count of scopes
#define MAX_LOADED_SCOPES 100

// States of all possible scopes
extern bool loaded_scopes[MAX_LOADED_SCOPES];


// Inits all necessary txm resources
void txm_init();

// Frees all txm resources. if 'free_tex' set to 1 also frees al its textures
void txm_free(bool free_tex);

// Returns the texture with the specified identifier.
// If the texture with the specified identifier cannot be found, NULL is returned
texture_t* txm_get(size_t id);

// Adds new texture to texture manager
void txm_push(size_t id, size_t scope, texture_t* tex);

// Loads all textures with specified scope
void txm_load_scope(size_t scope);

// Unloads all textures with specified scope
void txm_unload_scope(size_t scope);

#endif //DEEPCARS_TXM_H
