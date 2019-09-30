//
// Created by maxim on 9/11/19.
//

#ifndef ZOMBOID3D_MODEL_H
#define ZOMBOID3D_MODEL_H

#define GL_GLEXT_PROTOTYPES

#include <malloc.h>
#include <math.h>
#include "GL/gl.h"

typedef struct _sphere {
    float* data;

    float radius;
    int stackCount;
    int sectorCount;

    GLuint VAO;
    GLuint VBO;

    size_t count;

} sphere_t;

sphere_t* m_sphere(int stackCount, int sectorCount, float radius);
void m_draw_sphere(sphere_t* sphere);

#endif //ZOMBOID3D_MODEL_H
