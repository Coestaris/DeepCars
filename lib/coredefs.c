//
// Created by maxim on 8/31/19.
//

#ifdef __GNUC__
#pragma implementation "structs.h"
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#include "coredefs.h"

#include <GL/gl.h>
#include <stdarg.h>

#define CD_LOG(format, ...) DC_LOG("coredefs.c", format, __VA_ARGS__)
#define CD_ERROR(format, ...) DC_ERROR("coredefs.c", format, __VA_ARGS__)

#if VERBOSE == true

//
// __message
//
void __message(const char* prefix, const char* format, ...)
{
   va_list argp;
   va_start(argp, format);

   printf("[%s]: ", prefix);
   vprintf(format, argp);
   putchar('\n');

   va_end(argp);
}

//
// __error
//
void __error(const char* prefix, const char* file, size_t line, const char* format, ...)
{
   va_list argp;
   va_start(argp, format);

   printf("[%s][ERROR]: ", prefix);
   vprintf(format, argp);
   putchar('\n');
   printf("[%s][ERROR]: Error occurred in file \"%s\" at line %li", prefix, file, line);

   fflush(stdout);
   fflush(stdin);

   va_end(argp);

   abort();
}

#endif

#if DEBUG_LEVEL != 0

//
// __gl_check
//
void __gl_check(const char* line, int line_index, const char* file)
{
   static GLenum error;
   if((error = glGetError()) != GL_NO_ERROR)
   {
      const char* error_name = NULL;
      switch(error)
      {
         case GL_INVALID_ENUM: error_name = "GL_INVALID_ENUM";
            break;
         case GL_INVALID_VALUE: error_name = "GL_INVALID_VALUE";
            break;
         case GL_INVALID_OPERATION: error_name = "GL_INVALID_OPERATION";
            break;
         case GL_STACK_OVERFLOW: error_name = "GL_STACK_OVERFLOW";
            break;
         case GL_STACK_UNDERFLOW: error_name = "GL_STACK_UNDERFLOW";
            break;
         case GL_OUT_OF_MEMORY: error_name = "GL_OUT_OF_MEMORY";
            break;
         case GL_INVALID_FRAMEBUFFER_OPERATION: error_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
         case GL_CONTEXT_LOST: error_name = "GL_CONTEXT_LOST";
            break;
         case GL_TABLE_TOO_LARGE: error_name = "GL_TABLE_TOO_LARGE";
            break;
         default:
            error_name = "unknown error";
      }

      printf("[GL ERROR]: Error type: %i (%s) occurred while processing \"%s\"\n[GL ERROR]: At %s at line %i",
              error, error_name, line, file, line_index);

      // stdout не успевает высерать все сообщение.
      fflush(stdout);
      fflush(stdin);

      abort();
   }
}

#endif

//
// __check_malloc
//
inline void* __check_malloc(size_t size, size_t line_index, const char* file)
{
   void* ptr = DEEPCARS_MALLOC_FUNCTION(size);
   if(!ptr)
      CD_ERROR("Unable to allocate %li bytes. Allocation in %s at %li line",
            size, file, line_index);

   return ptr;
}

//
// __check_realloc
//
inline void* __check_realloc(void* ptr, size_t size, size_t line_index, const char* file)
{
   void* new_ptr = DEEPCARS_REALLOC_FUNCTION(ptr, size);
   if(!new_ptr)
      CD_ERROR("Unable to reallocate %p to %li bytes. Allocation in %s at %li line", ptr,
            size, file, line_index);

   return new_ptr;
}

#pragma clang diagnostic pop