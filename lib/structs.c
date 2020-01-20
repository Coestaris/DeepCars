//
// Created by maxim on 8/31/19.
//

#ifdef __GNUC__
#pragma implementation "structs.h"
#endif

#include <GL/gl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#include "structs.h"

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

//
// vec2f
//
inline vec2f_t vec2f(double_t x, double_t y)
{
   vec2f_t v = {x, y};
   return v;
}

//
// vec3f
//
inline vec3f_t vec3f(double_t x, double_t y, double_t z)
{
   vec3f_t v = {x, y, z};
   return v;
}

//
// list_push
//
inline void list_push(list_t* list, void* object)
{
   // current collection if full increase its size in 1.5 times
   if (list->count > list->max_size - 1)
   {
      if (list->max_size == LIST_BOOTSTRAP_SIZE) {
         void** new_collection = malloc(sizeof(void*) * ++list->max_size);
         memcpy(new_collection, list->collection, list->count);
         list->collection = new_collection;
      } else {
         size_t newLen = (int) ((float) list->max_size * 1.5f);
         list->collection = realloc(list->collection, sizeof(void*) * newLen);
         list->max_size = newLen;
      }
   }
   list->collection[list->count++] = object;
}

//
// list_free_elements
//
void list_free_elements(list_t* list)
{
   for (size_t i = 0; i < list->count; i++)
      free(list->collection[i]);
   list->count = 0;
}

//
// list_free
//
void list_free(list_t* list)
{
   if (list->collection != list->bootstrap) {
      free(list->collection);
   }
   free(list);
}

//
// list_remove
//
void list_remove(list_t* list, void* object)
{
   //todo
}

//
// list_create
//
list_t* list_create()
{
   list_t* list = malloc(sizeof(list_t));
   list->count = 0;
   list->max_size = LIST_BOOTSTRAP_SIZE;
   list->collection = list->bootstrap;

   memset(list->bootstrap, 0, sizeof(list->bootstrap));

   return list;
}