//
// Created by maxim on 8/31/19.
//

#ifndef DEEPCARS_COREDEFS_H
#define DEEPCARS_COREDEFS_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>

#include "list.h"
#include "vectors.h"

// Output debug information
#define VERBOSE true
#define DEBUG_LEVEL 2 // 2 - glCall, glPCall. 1 - glCall

#if DEBUG_LEVEL >= 2

   #define GL_CALL(expr) { expr; gl_check(#expr, __LINE__, __FILE__); }
   #define GL_PCALL(expr) { expr; gl_check(#expr, __LINE__, __FILE__); }

#elif DEBUG_LEVEL == 1

   #define GL_CALL(expr) { expr; gl_check(#expr, __LINE__, __FILE__); }
   #define GL_PCALL(expr) {expr;}

#else

   #define GL_CALL(expr) expr
   #define GL_PCALL(expr) expr

#endif

#if VERBOSE == true

   #define DC_LOG(prefix, format, ...) {_log(prefix, format, __VA_ARGS__);}
   #define DC_ERROR(prefix, format, ...) {_error(prefix, __FILE__, __LINE__, format, __VA_ARGS__);}

   void _log(const char* prefix, const char* format, ...);
   void _error(const char* prefix, const char* file, size_t line, const char* format, ...);

#else

   #define DC_LOG(prefix, format, ...) {};
   #define DC_ERROR(prefix, format, ...) {};

#endif

#if DEBUG_LEVEL > 0
   void gl_check(const char* line, int line_index, const char* file);
#endif

#ifndef DEEPCARS_MALLOC
   #define DEEPCARS_MALLOC malloc
#endif

#ifndef DEEPCARS_FREE
   #define DEEPCARS_FREE(x) { free(x); x = NULL; }
#endif

#endif //DEEPCARS_COREDEFS_H
