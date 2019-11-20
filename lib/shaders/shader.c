//
// Created by maxim on 3/6/19.
//

#ifdef __GNUC__
#pragma implementation "shader.h"
#endif
#include "shader.h"

// Builds shader form string
void sh_load_shader_src(const char* sz_shader_src, GLuint shader)
{
   GLchar* string_part[1];
   string_part[0] = (GLchar*) sz_shader_src;

   glShaderSource(shader, 1, (const GLchar**) string_part, NULL);
}

// Builds shader from file
int sh_load_shader_file(const char* filename, GLuint shader)
{
   FILE* f = fopen(filename, "r");
   if (!f) return 0;

   fseek(f, 0, SEEK_END);

   size_t size = (size_t) ftell(f);
   fseek(f, 0, SEEK_SET);

   char* rawInput = malloc(size + 1);
   memset(rawInput, 0, size + 1);
   fread(rawInput, size, size, f);

   fclose(f);

   sh_load_shader_src((const char*) rawInput, shader);

   free(rawInput);
   return 1;
}

// Builds shader and loads it to a GPU memory
int sh_load(shader_t* sh)
{
   GLuint vertex_shader;
   GLuint fragment_shader;

   vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

   if (vertex_shader == 0 || fragment_shader == 0)
   {
      printf("[shader.c]: Unable to create shaders\n");
      return 0;
   }

   if (!sh_load_shader_file(sh->fragment_path, fragment_shader))
   {
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      printf("[shader.c][ERROR]: The fragment shader at %s could not be found.\n",
              sh->fragment_path);
      return 0;
   }

   if (!sh_load_shader_file(sh->vertex_path, vertex_shader))
   {
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      printf("[shader.c][ERROR]: The vertex shader at %s could not be found.\n",
              sh->vertex_path);
      return 0;
   }

   glCompileShader(vertex_shader);
   glCompileShader(fragment_shader);

   GLint test_val;

   glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &test_val);
   if (test_val == GL_FALSE)
   {
      char infoLog[1024];
      glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
      printf("[shader.c][ERROR]: The vertex shader at %s failed to compile with the following error:\n%s\n",
             sh->vertex_path, infoLog);
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      return 0;
   }

   glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &test_val);
   if (test_val == GL_FALSE)
   {
      char infoLog[1024];
      glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
      printf("[shader.c][ERROR]: The fragment shader at %s failed to compile with the following error:\n%s\n",
             sh->fragment_path, infoLog);
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      return 0;
   }

   sh->prog_id = glCreateProgram();

   glAttachShader(sh->prog_id, vertex_shader);
   glAttachShader(sh->prog_id, fragment_shader);

   glLinkProgram(sh->prog_id);

   glGetProgramiv(sh->prog_id, GL_LINK_STATUS, &test_val);
   if (test_val == GL_FALSE)
   {
      char infoLog[1024];
      glGetProgramInfoLog(sh->prog_id, 1024, NULL, infoLog);
      printf("[shader.c][ERROR]: The programs %s and %s failed to link with the following errors:\n%s\n",
             sh->vertex_path, sh->fragment_path, infoLog);
      glDeleteProgram(sh->prog_id);
      return 0;

   }

   glDeleteShader(vertex_shader);
   glDeleteShader(fragment_shader);
   return 1;
}

//
// sh_create
//
shader_t* sh_create(char* vertexPath, char* fragmentPath)
{
   shader_t* sh = malloc(sizeof(shader_t));
   sh->fragment_path = fragmentPath;
   sh->vertex_path = vertexPath;
   sh->prog_id = 0;

   if (!sh_load(sh))
   {
      exit(EXIT_FAILURE);
   }

   printf("[shader.c]: Loaded shader. vertex path: %s, fragment path: %s\n",
           vertexPath, fragmentPath);
   return sh;
}

//
// sh_free
//
void sh_free(shader_t* sh)
{
   printf("[shader.c]: Freed shader. vertex path: %s, fragment path: %s\n",
           sh->vertex_path, sh->fragment_path);

   glDeleteProgram(sh->prog_id);

   if (sh->uniform_locations)
      free(sh->uniform_locations);

   free(sh);
}

//
// sh_use
//
inline void sh_use(shader_t* sh)
{
   if (!sh) glUseProgram(0);
   else glUseProgram(sh->prog_id);
}

void sh_set_int(shader_t* sh, GLint location, int value)
{
   glUniform1i(location, value);
}

void sh_set_float(shader_t* sh, GLint location, float value)
{
   glUniform1f(location, value);
}

void sh_set_mat4(shader_t* sh, GLint location, mat4 value)
{
   glUniformMatrix4fv(location, 1, true, value);
}

void sh_set_vec3v(shader_t* sh, GLint location, float a, float b, float c)
{
   glUniform3f(location, a, b, c);
}

void sh_set_vec3(shader_t* sh, GLint location, vec4 v)
{
   glUniform3fv(location, 3, v);
}

void sh_set_vec4v(shader_t* sh, GLint locatione, float a, float b, float c, float d)
{
   glUniform4f(locatione, a, b, c, d);
}

void sh_set_vec4(shader_t* sh, GLint location, vec4 v)
{
   glUniform3fv(location, 4, v);
}

void sh_nset_int(shader_t* sh, const char* name, int value)
{
   sh_set_int(sh, glGetUniformLocation(sh->prog_id, name), value);
}

void sh_nset_float(shader_t* sh, const char* name, float value)
{
   sh_set_float(sh, glGetUniformLocation(sh->prog_id, name), value);
}

void sh_nset_mat4(shader_t* sh, const char* name, mat4 value)
{
   sh_set_mat4(sh, glGetUniformLocation(sh->prog_id, name), value);
}

void sh_nset_vec3v(shader_t* sh, const char* name, float a, float b, float c)
{
   sh_set_vec3v(sh, glGetUniformLocation(sh->prog_id, name), a, b, c);
}

void sh_nset_vec3(shader_t* sh, const char* name, vec4 v)
{
   sh_set_vec3(sh, glGetUniformLocation(sh->prog_id, name), v);
}

void sh_nset_vec4v(shader_t* sh, const char* name, float a, float b, float c, float d)
{
   sh_set_vec4v(sh, glGetUniformLocation(sh->prog_id, name), a, b, c, d);
}

void sh_nset_vec4(shader_t* sh, const char* name, vec4 v)
{
   sh_set_vec4(sh, glGetUniformLocation(sh->prog_id, name), v);
}

//
// sh_info
//
void sh_info(shader_t* sh)
{
   GLint i;
   GLint count;

   GLint size; // size of the variable
   GLenum type; // type of the variable (float, vec3 or mat4, etc)

   const GLsizei bufSize = 32; // maximum name length
   GLchar name[bufSize]; // variable name in GLSL
   GLsizei length; // name length

   glGetProgramiv(sh->prog_id, GL_ACTIVE_ATTRIBUTES, &count);
   printf("[shader.c]: Active Attributes: %d\n", count);

   for (i = 0; i < count; i++)
   {
      glGetActiveAttrib(sh->prog_id, (GLuint) i, bufSize, &length, &size, &type, name);

      printf("[shader.c]: Attribute #%d Type: %u Name: %s\n", i, type, name);
   }

   glGetProgramiv(sh->prog_id, GL_ACTIVE_UNIFORMS, &count);
   printf("[shader.c]: Active Uniforms: %d\n", count);

   for (i = 0; i < count; i++)
   {
      glGetActiveUniform(sh->prog_id, (GLuint) i, bufSize, &length, &size, &type, name);

      printf("[shader.c]: Uniform #%d Type: %u Name: %s\n", i, type, name);
   }
}