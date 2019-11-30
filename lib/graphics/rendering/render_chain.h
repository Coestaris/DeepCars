//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_CHAIN_H
#define DEEPCARS_RENDER_CHAIN_H

#include <stdbool.h>
#include "../../structs.h"
#include "../../object.h"
#include "../../shaders/shader.h"

typedef struct _render_chain {

   list_t stages;
   GLint* FBOs;
   GLint* Textures;
   void* data;

} render_chain_t;

#endif //DEEPCARS_RENDER_CHAIN_H
