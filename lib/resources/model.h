//
// Created by maxim on 9/11/19.
//

#ifndef ZOMBOID3D_MODEL_H
#define ZOMBOID3D_MODEL_H

#define GL_GLEXT_PROTOTYPES

#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include "GL/gl.h"

typedef struct _model {


} model_t;

model_t* m_load(const char* filename);
void m_free(model_t* model);

#endif //ZOMBOID3D_MODEL_H
