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

#include "../graphics/gmath.h"
#include "../graphics/material.h"

typedef struct _shader
{
    char* vertexPath;
    char* fragmentPath;
    GLint progID;

} shader_t;

shader_t* sh_create(char* vertexPath, char* fragmentPath);
void sh_free(shader_t* sh);
void sh_setInt(shader_t* sh, const char* name, int value);
void sh_setFloat(shader_t* sh, const char* name, float value);
void sh_setMat4(shader_t* sh, const char* name, mat4 value);
void sh_setVec3v(shader_t* sh, const char* name, float a, float b, float c);
void sh_setVec3(shader_t* sh, const char* name,  vec4 v);
void sh_setVec4v(shader_t* sh, const char* name, float a, float b, float c, float d);
void sh_setVec4(shader_t* sh, const char* name, vec4 v);
void sh_setMaterial(shader_t* sh, material_t* material);
int sh_load(shader_t* sh);
void sh_use(shader_t* sh);
void sh_info(shader_t* sh);

#endif //ZOMBOID3D_SHADER_H
