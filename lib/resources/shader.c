//
// Created by maxim on 3/6/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#ifdef __GNUC__
#pragma implementation "shader.h"
#endif
#include "shader.h"

#define SH_LOG(format, ...) DC_LOG("shader.c", format, __VA_ARGS__)
#define SH_ERROR(format, ...) DC_ERROR("shader.c", format, __VA_ARGS__)

// Builds shader and loads it to a GPU memory
void sh_compile_s(shader_t* sh,
                  uint8_t* vertex_source,   GLint vertex_len,
                  uint8_t* geometry_source, GLint geometry_len,
                  uint8_t* fragment_source, GLint fragment_len)
{
   GLuint vertex_shader   = 0;
   GLuint fragment_shader = 0;
   GLuint geometry_shader = 0;
   GLint test_val;

   if (vertex_source)
   {
      vertex_shader = glCreateShader(GL_VERTEX_SHADER);
      if (!vertex_shader)
      {
         SH_ERROR("Unable to create vertex shader",0);
      }
      sh->programs |= 0x1u;
   }

   if (geometry_source)
   {
      geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
      if (!geometry_shader)
      {
         SH_ERROR("Unable to create geometry shader",0);
      }
      sh->programs |= 0x2u;
   }

   if (fragment_source)
   {
      fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
      if (!fragment_shader)
      {
         SH_ERROR("Unable to create fragment shader",0);
      }
      sh->programs |= 0x4u;
   }

   if (vertex_source)
   {
      GL_CALL(glShaderSource(vertex_shader, 1, (const GLchar**)&vertex_source, &vertex_len))
      GL_CALL(glCompileShader(vertex_shader));
      GL_CALL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &test_val));
      if (test_val == GL_FALSE)
      {
         char info_log[1024];
         GL_CALL(glGetShaderInfoLog(vertex_shader, 1024, NULL, info_log));
         SH_ERROR("The vertex shader of \"%s\" failed to compile with the following error:\n%s",
                sh->name, info_log);
      }
   }

   if (geometry_source)
   {
      GL_CALL(glShaderSource(geometry_shader, 1, (const GLchar**)&geometry_source, &geometry_len))
      GL_CALL(glCompileShader(geometry_shader));
      GL_CALL(glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &test_val));
      if (test_val == GL_FALSE)
      {
         char info_log[1024];
         GL_CALL(glGetShaderInfoLog(geometry_shader, 1024, NULL, info_log));
         SH_ERROR("The fragment shader of \"%s\" failed to compile with the following error:\n%s",
                sh->name, info_log);
      }
   }

   if(fragment_source)
   {
      GL_CALL(glShaderSource(fragment_shader, 1, (const GLchar**)&fragment_source, &fragment_len))
      GL_CALL(glCompileShader(fragment_shader));
      GL_CALL(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &test_val))
      if (test_val == GL_FALSE)
      {
         char info_log[1024];
         GL_CALL(glGetShaderInfoLog(fragment_shader, 1024, NULL, info_log));
         SH_ERROR("The fragment shader of \"%s\" failed to compile with the following error:\n%s",
                sh->name, info_log);
      }

   }
   sh->prog_id = glCreateProgram();
   if(!sh->prog_id)
   {
      SH_ERROR("Unable to create program",0);
   }

   if(fragment_source)
      GL_CALL(glAttachShader(sh->prog_id, vertex_shader));
   if(geometry_source)
      GL_CALL(glAttachShader(sh->prog_id, geometry_shader));
   if(fragment_source)
      GL_CALL(glAttachShader(sh->prog_id, fragment_shader));

   GL_CALL(glLinkProgram(sh->prog_id));

   GL_CALL(glGetProgramiv(sh->prog_id, GL_LINK_STATUS, &test_val));
   if (test_val == GL_FALSE)
   {
      char info_log[1024];
      GL_CALL(glGetProgramInfoLog(sh->prog_id, 1024, NULL, info_log));
      SH_ERROR("The shader \"%s\" failed to link with the following errors:\n%s",
             sh->name, info_log);
   }

   if(vertex_source)
      GL_CALL(glDeleteShader(vertex_shader));
   if(geometry_source)
      GL_CALL(glDeleteShader(geometry_shader));
   if(fragment_source)
      GL_CALL(glDeleteShader(fragment_shader));

   char buff[100];
   if(vertex_source)
      sprintf(buff, "%s, ", "vertex");
   if(geometry_source)
      sprintf(buff + strlen(buff), "%s, ", "geometry");
   if(fragment_source)
      sprintf(buff + strlen(buff), "%s, ", "fragment");
   buff[strlen(buff) - 2] = '\0';

   SH_LOG("Loaded shader \"%s\" (%s)", sh->name, buff);
}

void sh_compile(shader_t* sh, char* vertex_path, char* geometry_path, char* fragment_path)
{
   uint8_t* vertex_source   = NULL;
   uint8_t* geometry_source = NULL;
   uint8_t* fragment_source = NULL;
   size_t vertex_len        = 0;
   size_t geometry_len      = 0;
   size_t fragment_len      = 0;

   if(vertex_path)
   {
      FILE* f = fopen(vertex_path, "rb");
      if(!f)
      {
         SH_ERROR("Unable to open file %s", vertex_path);
      }
      fseek(f, 0, SEEK_END);
      vertex_len = ftell(f);
      fseek(f, 0, SEEK_SET);
      vertex_source = malloc(vertex_len + 1);
      fread(vertex_source, vertex_len, 1, f);
      vertex_source[vertex_len] = 0;
      fclose(f);
   }
   if(geometry_path)
   {
      FILE* f = fopen(geometry_path, "rb");
      if(!f)
      {
         SH_ERROR("Unable to open file %s", geometry_path);
      }
      fseek(f, 0, SEEK_END);
      geometry_len = ftell(f);
      fseek(f, 0, SEEK_SET);
      geometry_source = malloc(geometry_len + 1);
      fread(geometry_source, geometry_len, 1, f);
      geometry_source[geometry_len] = 0;
      fclose(f);
   }
   if(fragment_path)
   {
      FILE* f = fopen(fragment_path, "rb");
      if(!f)
      {
         SH_ERROR("Unable to open file %s", fragment_path);
      }
      fseek(f, 0, SEEK_END);
      fragment_len = ftell(f);
      fseek(f, 0, SEEK_SET);
      fragment_source = malloc(fragment_len + 1);
      fread(fragment_source, fragment_len, 1, f);
      fragment_source[fragment_len] = 0;
      fclose(f);
   }

   sh_compile_s(sh,
           vertex_source, vertex_len,
           geometry_source, geometry_len,
           fragment_source, fragment_len);

   if(vertex_source) free(vertex_source);
   if(geometry_source) free(geometry_source);
   if(fragment_source) free(fragment_source);
}

//
// sh_create
//
shader_t* sh_create(char* name)
{
   shader_t* sh = malloc(sizeof(shader_t));
   sh->prog_id = 0;
   sh->name = name;
   sh->programs = 0;
   return sh;
}

//
// sh_free
//
void sh_free(shader_t* sh)
{
   SH_LOG("Freed shader \"%s\"", sh->name);
   GL_CALL(glDeleteProgram(sh->prog_id))

   free(sh->name);
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

inline void sh_set_vec2v(GLint location, float a, float b)
{
   GL_PCALL(glUniform2f(location, a, b));
}

inline void sh_set_vec2(GLint location, vec4 v)
{
   sh_set_vec2v(location, v[0], v[1]);
}

inline void sh_set_vec3v(GLint location, float a, float b, float c)
{
   GL_PCALL(glUniform3f(location, a, b, c));
}

inline void sh_set_vec3(GLint location, vec4 v)
{
   sh_set_vec3v(location, v[0], v[1], v[2]);
}

inline void sh_set_vec4v(GLint location, float a, float b, float c, float d)
{
   GL_PCALL(glUniform4f(location, a, b, c, d));
}

inline void sh_set_vec4(GLint location, vec4 v)
{
   sh_set_vec4v(location, v[0], v[1], v[2], v[3]);
}

inline void sh_nset_int(shader_t* sh, const char* name, int value)
{
   GLuint location = glGetUniformLocation(sh->prog_id, name);
#if DEBUG_LEVEL == 3
   assert(location != -1);
#endif
   sh_set_int(location, value);
}

inline void sh_nset_float(shader_t* sh, const char* name, float value)
{
   GLuint location = glGetUniformLocation(sh->prog_id, name);
#if DEBUG_LEVEL == 3
   assert(location != -1);
#endif
   sh_set_float(location, value);
}

inline void sh_nset_mat4(shader_t* sh, const char* name, mat4 value)
{
   GLuint location = glGetUniformLocation(sh->prog_id, name);
#if DEBUG_LEVEL == 3
   assert(location != -1);
#endif
   sh_set_mat4(location, value);
}

inline void sh_nset_vec2v(shader_t* sh, const char* name, float a, float b)
{
   GLuint location = glGetUniformLocation(sh->prog_id, name);
#if DEBUG_LEVEL == 3
   assert(location != -1);
#endif
   sh_set_vec2v(location, a, b);
}

inline void sh_nset_vec2(shader_t* sh, const char* name, vec4 v)
{
   sh_nset_vec2v(sh, name, v[0], v[1]);
}


inline void sh_nset_vec3v(shader_t* sh, const char* name, float a, float b, float c)
{
   GLuint location = glGetUniformLocation(sh->prog_id, name);
#if DEBUG_LEVEL == 3
   assert(location != -1);
#endif
   sh_set_vec3v(location, a, b, c);
}

inline void sh_nset_vec3(shader_t* sh, const char* name, vec4 v)
{
   sh_nset_vec3v(sh, name, v[0], v[1], v[2]);
}

inline void sh_nset_vec4v(shader_t* sh, const char* name, float a, float b, float c, float d)
{
   GLuint location = glGetUniformLocation(sh->prog_id, name);
#if DEBUG_LEVEL == 3
   assert(location != -1);
#endif
   sh_set_vec4v(location, a, b, c, d);
}

inline void sh_nset_vec4(shader_t* sh, const char* name, vec4 v)
{
   sh_nset_vec4v(sh, name, v[0], v[1], v[2], v[3]);
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

   SH_LOG("[sh_info]: Shader: \"%s\"", sh->name);
   SH_LOG("[sh_info]: Shader progID: %i", sh->prog_id);

   GL_CALL(glGetProgramiv(sh->prog_id, GL_ACTIVE_ATTRIBUTES, &count));
   SH_LOG("[sh_info]: Active Attributes: %d", count);

   for (i = 0; i < count; i++)
   {
      GL_CALL(glGetActiveAttrib(sh->prog_id, (GLuint) i, buf_size, &length, &size, &type, name));
      SH_LOG("[sh_info]: Attribute %d:[type: %u, name: \"%s\"]", i, type, name);
   }

   GL_CALL(glGetProgramiv(sh->prog_id, GL_ACTIVE_UNIFORMS, &count));
   SH_LOG("sh_info]: Active Uniforms: %d", count);

   for (i = 0; i < count; i++)
   {
      GL_CALL(glGetActiveUniform(sh->prog_id, (GLuint) i, buf_size, &length, &size, &type, name));
      SH_LOG("[sh_info]: Uniform %d:[type: %u, name: \"%s\"]", i, type, name);
   }

   SH_LOG("",0);
}
#pragma clang diagnostic pop