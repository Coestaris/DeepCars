//
// Created by maxim on 3/6/19.
//

#include <stdbool.h>
#include "shader.h"

shader_t* sh_create(char* vertexPath, char* fragmentPath)
{
    shader_t* sh = malloc(sizeof(shader_t));
    sh->fragmentPath = fragmentPath;
    sh->vertexPath = vertexPath;
    sh->progID = 0;

    if (!sh_load(sh)) return NULL;

    printf("[shader.c]: Loaded shader. vertex path: %s, fragment path: %s\n", vertexPath, fragmentPath);
    return sh;
}

void sh_free(shader_t* sh)
{
    glDeleteProgram(sh->progID);
    free(sh);
}

void loadShaderSrc(const char* szShaderSrc, GLuint shader)
{
    GLchar* fsStringPtr[1];
    fsStringPtr[0] = (GLchar*) szShaderSrc;

    glShaderSource(shader, 1, (const GLchar**) fsStringPtr, NULL);
}

int loadShaderFile(const char* szFile, GLuint shader)
{
    FILE* f = fopen(szFile, "r");
    if (!f) return 0;

    fseek(f, 0, SEEK_END);

    size_t size = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    char* rawInput = malloc(size + 1);
    memset(rawInput, 0, size + 1);
    fread(rawInput, size, size, f);

    fclose(f);

    loadShaderSrc((const char*) rawInput, shader);

    free(rawInput);

    return 1;
}

int sh_load(shader_t* sh)
{
    GLuint vertexShader;
    GLuint fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        printf("[shader.c]: Unable to create shaders\n");
        return 0;
    }

    if (!loadShaderFile(sh->fragmentPath, fragmentShader))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        printf("[shader.c][ERROR]: The fragment shader at %s could not be found.\n", sh->fragmentPath);
        return 0;
    }

    if (!loadShaderFile(sh->vertexPath, vertexShader))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        printf("[shader.c][ERROR]: The vertex shader at %s could not be found.\n", sh->vertexPath);
        return 0;
    }

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLint testVal;

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &testVal);
    if (testVal == GL_FALSE)
    {
        char infoLog[1024];
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        printf("[shader.c][ERROR]: The vertex shader at %s failed to compile with the following error:\n%s\n",
               sh->vertexPath, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &testVal);
    if (testVal == GL_FALSE)
    {
        char infoLog[1024];
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        printf("[shader.c][ERROR]: The fragment shader at %s failed to compile with the following error:\n%s\n",
               sh->fragmentPath, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    sh->progID = glCreateProgram();

    glAttachShader(sh->progID, vertexShader);
    glAttachShader(sh->progID, fragmentShader);

    glLinkProgram(sh->progID);

    glGetProgramiv(sh->progID, GL_LINK_STATUS, &testVal);
    if (testVal == GL_FALSE)
    {
        char infoLog[1024];
        glGetProgramInfoLog(sh->progID, 1024, NULL, infoLog);
        printf("[shader.c][ERROR]: The programs %s and %s failed to link with the following errors:\n%s\n",
               sh->vertexPath, sh->fragmentPath, infoLog);
        glDeleteProgram(sh->progID);
        return 0;

    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return 1;
}

inline void sh_use(shader_t* sh)
{
    glUseProgram(sh->progID);
}

void sh_setInt(shader_t* sh, GLint location, int value)
{
    glUniform1i(location, value);
}

void sh_setFloat(shader_t* sh, GLint location, float value)
{
    glUniform1f(location, value);
}

void sh_setMat4(shader_t* sh, GLint location, mat4 value)
{
    glUniformMatrix4fv(location,1, true, value);
}

void sh_setVec3v(shader_t* sh, GLint location, float a, float b, float c)
{
    glUniform3f(location, a, b, c);
}

void sh_setVec3(shader_t* sh, GLint location,  vec4 v)
{
    glUniform3fv(location, 3, v);
}

void sh_setVec4v(shader_t* sh, GLint location, float a, float b, float c, float d)
{
    glUniform4f(location, a, b, c, d);
}

void sh_setVec4(shader_t* sh, GLint location, vec4 v)
{
    glUniform3fv(location, 4, v);
}

void sh_nsetInt(shader_t* sh, const char* name, int value)
{
    sh_setInt(sh, glGetUniformLocation(sh->progID, name), value);
}

void sh_nsetFloat(shader_t* sh, const char* name, float value)
{
    sh_setFloat(sh, glGetUniformLocation(sh->progID, name), value);
}

void sh_nsetMat4(shader_t* sh, const char* name, mat4 value)
{
    sh_setMat4(sh, glGetUniformLocation(sh->progID, name), value);
}

void sh_nsetVec3v(shader_t* sh, const char* name, float a, float b, float c)
{
    sh_setVec3v(sh, glGetUniformLocation(sh->progID, name), a, b, c);
}

void sh_nsetVec3(shader_t* sh, const char* name,  vec4 v)
{
    sh_setVec3(sh, glGetUniformLocation(sh->progID, name), v);
}

void sh_nsetVec4v(shader_t* sh, const char* name, float a, float b, float c, float d)
{
    sh_setVec4v(sh, glGetUniformLocation(sh->progID, name), a, b, c, d);
}

void sh_nsetVec4(shader_t* sh, const char* name, vec4 v)
{
    sh_setVec4(sh, glGetUniformLocation(sh->progID, name), v);
}

void sh_info(shader_t* sh)
{
    GLint i;
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 32; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(sh->progID, GL_ACTIVE_ATTRIBUTES, &count);
    printf("Active Attributes: %d\n", count);

    for (i = 0; i < count; i++)
    {
        glGetActiveAttrib(sh->progID, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
    }

    glGetProgramiv(sh->progID, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(sh->progID, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
    }
}