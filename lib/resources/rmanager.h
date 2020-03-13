//
// Created by maxim on 1/1/20.
//

#ifndef DEEPCARS_RMANAGER_H
#define DEEPCARS_RMANAGER_H

#include "../coredefs.h"

// Type of resource stored in the resource manager
typedef enum _resource_type {
   TEXTURE,
   MODEL,
   MATERIAL,
   FONT

} resource_type_t;

// Allocate all necessary by rmanager data
void rm_init(void);

// Get resource by it's ID. If there's no resource with specified ID error will be thrown
void* rm_get(resource_type_t type, uint32_t id);

// Get resource by it's name. If there's no resource with specified name error will be thrown
void* rm_getn(resource_type_t type, const char* name);

// Try to get resource with specified ID, if it cannot be found function returns NULL
void* rm_get_try(resource_type_t type, uint32_t id);

// Try to get resource with specified name, if it cannot be found function returns NULL
void* rm_getn_try(resource_type_t type, const char* name);

// Push resource to a rmanager. If id equals to -1 it will be assigned automatically
void rm_push(resource_type_t type, void* data, int32_t id);

// Release all used data and frees all resources
void rm_release(bool free_tex, bool free_models, bool free_mats, bool free_fonts);

#endif //DEEPCARS_RMANAGER_H
