//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_TEXTURE_H
#define DEEPCARS_TEXTURE_H

#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>

#include <GL/gl.h>

#include "../../oil/oil.h"
#include "../structs.h"

typedef struct _texture
{
    const char* fn;
    GLuint texID;

    vec2f_t center;

    int width;
    int height;

    texData* data;

} texture_t;

texture_t* t_create(const char* fn, vec2f_t center);
void t_free(texture_t* tex);

void t_unload(texture_t* tex);
void t_load(texture_t* tex);

void t_bind(texture_t* tex);

#endif //DEEPCARS_TEXTURE_H
