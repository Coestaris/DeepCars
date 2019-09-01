//
// Created by maxim on 8/31/19.
//

#ifndef ZOMBOID3D_SHADER_H
#define ZOMBOID3D_SHADER_H

#define GL_GLEXT_PROTOTYPES

#include <X11/Xlib.h>
#include <GL/glx.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "../gmath.h"

typedef struct _shader
{
    char* vertexPath;
    char* fragmentPath;
    GLint progID;

} shader_t;

shader_t* sh_create(char* vertexPath, char* fragmentPath);
void sh_free(shader_t* shader);
void sh_setInt(shader_t* shader, const char* name, int value);
void sh_setFloat(shader_t* shader, const char* name, float value);
void sh_setMat4(shader_t* shader, const char* name, mat4 value);
int sh_load(shader_t* sh);
void sh_use(shader_t* sh);
void sh_info(shader_t* sh);

#endif //ZOMBOID3D_SHADER_H
