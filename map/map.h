//
// Created by maxim on 9/4/19.
//

#ifndef ZOMBOID3D_MAP_H
#define ZOMBOID3D_MAP_H

#include <stddef.h>
#include <malloc.h>
#include <stdio.h>

#include "../structs.h"

typedef struct _map_sector {
    vec2f_t a, b, c, d;
    float height;

} map_sector_t;

typedef struct _map {
    size_t sectorsCount;
    map_sector_t** sectors;

} map_t;

map_t* m_load(const char* fileName);

#endif //ZOMBOID3D_MAP_H
