//
// Created by maxim on 1/1/20.
//

#ifndef DEEPCARS_RMANAGER_H
#define DEEPCARS_RMANAGER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum _resource_type {
   TEXTURE,
   MODEL,
   MATERIAL,
   FONT

} resource_type_t;

void rm_init(void);

void* rm_get(resource_type_t type, uint32_t id);

void* rm_getn(resource_type_t type, const char* name);

void* rm_get_try(resource_type_t type, uint32_t id);

void* rm_getn_try(resource_type_t type, const char* name);

void rm_push(resource_type_t type, void* data, int32_t id);

void rm_free(bool free_tex, bool free_model, bool free_mat, bool free_font);

#endif //DEEPCARS_RMANAGER_H
