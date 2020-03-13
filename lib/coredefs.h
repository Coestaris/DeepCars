//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_COREDEFS_H
#define DEEPCARS_COREDEFS_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>

#include "list.h"
#include "vectors.h"


// Initial message parameters
#define DEEPCARS "DeepCars"
#define DEEPCARS_VERSION "0.1"
#define DEEPCARS_DEVS "Coestaris"


#ifndef VERBOSE
   // Output debug information. Activates *_LOG macro
   #define VERBOSE true
#endif


#ifndef DEBUG_LEVEL
   // 3 - Check uniform locations and previous
   // 2 - glPCall and previous
   // 1 - glCall, check malloc/realloc results
   // 0 - no debug checks provided
   #define DEBUG_LEVEL 3
#endif


#if DEBUG_LEVEL >= 1
   // Check for NULL allocated pointer, used only when DEBUG_LEVEL > 0
   void* __check_malloc(size_t size, size_t line_index, const char* file);
   // Check for NULL reallocated pointer, used only when DEBUG_LEVEL > 0
   void* __check_realloc(void* ptr, size_t size, size_t line_index, const char* file);
#endif


#if DEBUG_LEVEL >= 2

   #define ASSERT(expr) assert(expr)
   #define PASSERT(expr) assert(expr)

   // Check glGetError() after running specified expression.
   // Should be used for non performance critical functions
   #define GL_CALL(expr) { expr; __gl_check(#expr, __LINE__, __FILE__); }

   // Check glGetError() after running specified expression.
   // Should be used for performance critical functions
   #define GL_PCALL(expr) { expr; __gl_check(#expr, __LINE__, __FILE__); }

#elif DEBUG_LEVEL == 1

   #define ASSERT(expr) assert(expr)
   #define PASSERT(expr) {};

   // If DEBUG_LEVEL == 1 ignore OpenGL checks for performance critical functions
   #define GL_CALL(expr) { expr; __gl_check(#expr, __LINE__, __FILE__); }
   #define GL_PCALL(expr) {expr;}
#else

   #define ASSERT(expr) {}
   #define PASSERT(expr) {}

   // If DEBUG_LEVEL == 0 ignore all OpenGL checks
   #define GL_CALL(expr) {expr;}
   #define GL_PCALL(expr) {expr;}

#endif

// Provides ability to create custom error function just by specifying its prefix
#define DC_ERROR(prefix, format, ...) {__error(prefix, __FILE__, __LINE__, format, __VA_ARGS__);}

// Raises error and aborts program
void __error(const char* prefix, const char* file, size_t line, const char* format, ...);


#if VERBOSE == true
   // Provides ability to create custom log function just by specifying its prefix
   #define DC_LOG(prefix, format, ...) {__message(prefix, format, __VA_ARGS__);}
   // Prints message to a stdout
   void __message(const char* prefix, const char* format, ...);
#else
   // DC_LOG is doing nothing if VERBOSE set to false
   #define DC_LOG(prefix, format, ...) {};
#endif


#if DEBUG_LEVEL > 0
   // Checks OpenGL error code, if its not 0 raises error with specified debug data
   void __gl_check(const char* line, int line_index, const char* file);
#endif


// You can specify your custom memory allocation/deallocation
// functions by overriding this defines
#ifndef DEEPCARS_MALLOC_FUNCTION
   #define DEEPCARS_MALLOC_FUNCTION malloc
#endif
#ifndef DEEPCARS_REALLOC_FUNCTION
   #define DEEPCARS_REALLOC_FUNCTION realloc
#endif
#ifndef DEEPCARS_FREE_FUNCTION
   #define DEEPCARS_FREE_FUNCTION free
#endif


// If debug level set to 1 or greater checks return value of malloc,
// if it's NULL raises error
#if DEBUG_LEVEL >= 1
   #define DEEPCARS_MALLOC(x) __check_malloc(x, __LINE__, __FILE__)
#else
   #define DEEPCARS_MALLOC DEEPCARS_MALLOC_FUNCTION
#endif


// If debug level set to 1 or greater checks return value of realloc,
// if it's NULL raises error
#if DEBUG_LEVEL >= 1
   #define DEEPCARS_REALLOC(x, s) __check_realloc(x, s, __LINE__, __FILE__)
#else
   #define DEEPCARS_REALLOC DEEPCARS_REALLOC_FUNCTION
#endif


// If debug level set to 1 after freeing pointer sets it value to a null
#if DEBUG_LEVEL >= 1
   #define DEEPCARS_FREE(x) { DEEPCARS_FREE_FUNCTION(x); x = NULL; }
#else
   #define DEEPCARS_FREE DEEPCARS_FREE_FUNCTION
#endif


#pragma clang diagnostic pop

#endif //DEEPCARS_COREDEFS_H

