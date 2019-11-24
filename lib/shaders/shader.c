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

   GL_CALL(glDeleteShader(vertex_shader))
   GL_CALL(glDeleteShader(fragment_shader))
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

   GL_CALL(glDeleteProgram(sh->prog_id))

   if (sh->uniform_locations)
      free(sh->uniform_locations);

   free(sh);
}

//
// sh_use
//
inline void sh_use(shader_t* sh)
{
   if (!sh) GL_PCALL(glUseProgram(0))
   else GL_PCALL(glUseProgram(sh->prog_id));
}

inline void sh_set_int(GLint location, int value)
{
   GL_PCALL(glUniform1i(location, value));
}

inline void sh_set_float(GLint location, float value)
{
   GL_PCALL(glUniform1f(location, value));
}

inline void sh_set_mat4(GLint location, mat4 value)
{
   GL_PCALL(glUniformMatrix4fv(location, 1, true, value));
}

inline void sh_set_vec3v(GLint location, float a, float b, float c)
{
   GL_PCALL(glUniform3f(location, a, b, c));
}

inline void sh_set_vec3(GLint location, vec4 v)
{
   GL_PCALL(glUniform3fv(location, 3, v));
}

inline void sh_set_vec4v(GLint location, float a, float b, float c, float d)
{
   GL_PCALL(glUniform4f(location, a, b, c, d));
}

inline void sh_set_vec4(GLint location, vec4 v)
{
   GL_PCALL(glUniform3fv(location, 4, v));
}

inline void sh_nset_int(shader_t* sh, const char* name, int value)
{
   GL_PCALL(sh_set_int(glGetUniformLocation(sh->prog_id, name), value));
}

inline void sh_nset_float(shader_t* sh, const char* name, float value)
{
   GL_PCALL(sh_set_float(glGetUniformLocation(sh->prog_id, name), value));
}

inline void sh_nset_mat4(shader_t* sh, const char* name, mat4 value)
{
   GL_PCALL(sh_set_mat4(glGetUniformLocation(sh->prog_id, name), value));
}

inline void sh_nset_vec3v(shader_t* sh, const char* name, float a, float b, float c)
{
   GL_PCALL(sh_set_vec3v(glGetUniformLocation(sh->prog_id, name), a, b, c));
}

inline void sh_nset_vec3(shader_t* sh, const char* name, vec4 v)
{
   GL_PCALL(sh_set_vec3(glGetUniformLocation(sh->prog_id, name), v));
}

inline void sh_nset_vec4v(shader_t* sh, const char* name, float a, float b, float c, float d)
{
   GL_PCALL(sh_set_vec4v(glGetUniformLocation(sh->prog_id, name), a, b, c, d));
}

inline void sh_nset_vec4(shader_t* sh, const char* name, vec4 v)
{
   GL_PCALL(sh_set_vec4(glGetUniformLocation(sh->prog_id, name), v));
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

   const GLsizei buf_size = 32; // maximum name length
   GLchar name[buf_size]; // variable name in GLSL
   GLsizei length; // name length

   printf("[shader.c][sh_info]: Shader vertex path: %s, Shader fragment path: %s\n", sh->vertex_path, sh->fragment_path);
   printf("[shader.c][sh_info]: Shader progID: %i\n", sh->prog_id);

   GL_CALL(glGetProgramiv(sh->prog_id, GL_ACTIVE_ATTRIBUTES, &count));
   printf("[shader.c][sh_info]: Active Attributes: %d\n", count);

   for (i = 0; i < count; i++)
   {
      GL_CALL(glGetActiveAttrib(sh->prog_id, (GLuint) i, buf_size, &length, &size, &type, name));
      printf("[shader.c][sh_info]: Attribute %d:[type: %u, name: \"%s\"]\n", i, type, name);
   }

   GL_CALL(glGetProgramiv(sh->prog_id, GL_ACTIVE_UNIFORMS, &count));
   printf("[shader.c][sh_info]: Active Uniforms: %d\n", count);

   for (i = 0; i < count; i++)
   {
      GL_CALL(glGetActiveUniform(sh->prog_id, (GLuint) i, buf_size, &length, &size, &type, name));
      printf("[shader.c][sh_info]: Uniform %d:[type: %u, name: \"%s\"]\n", i, type, name);
   }

   printf("[shader.c][sh_info]: Uniform Locations: ");
   for(i = 0; i < 5; i++)
      printf("%i, ", sh->uniform_locations[i]);
   printf("\n");

}