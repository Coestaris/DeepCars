//
// Created by maxim on 8/31/19.
//

#ifdef __GNUC__
#pragma implementation "structs.h"
#endif
#include "coredefs.h"

#include <GL/gl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#if VERBOSE == true
void _log(const char* prefix, const char* format, ...)
{
   va_list argp;
   va_start(argp, format);

   printf("[%s]: ", prefix);
   vprintf(format, argp);
   putchar('\n');

   va_end(argp);
}

void _error(const char* prefix, const char* file, size_t line, const char* format, ...)
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
void gl_check(const char* line, int line_index, const char* file)
{
   GLenum error;
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

      fflush(stdout); // stdout не успевает высерать все сообщение.
      fflush(stdin);

      abort();
      //exit(EXIT_FAILURE);
   }
}
#endif
