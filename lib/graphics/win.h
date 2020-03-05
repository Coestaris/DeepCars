//
// Created by maxim on 8/26/19.
//

#ifndef DEEPCARS_WIN_H
#define DEEPCARS_WIN_H

#define GL_GLEXT_PROTOTYPES

#include "../coredefs.h"

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "gmath.h"
#include "camera.h"

// Major version of OpenGL context
#define GLX_DEFAULT_MAJOR_VERSION 3
// Minor version of OpenGL context
#define GLX_DEFAULT_MINOR_VERSION 0
// Flags of OpenGL context
#define GLX_DEFAULT_FLAGS         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB

// Stores all OpenGL information
typedef struct _win_info
{
   // XServer display to use
   Display* display;

   // XServer window to use
   Window win;

   // Used OpenGL context
   GLXContext context;

   // Used OpenGL color map
   Colormap color_map;

   // Window size
   uint16_t w;
   uint16_t h;

   // Caprion of the window
   const char* caption;

} win_info_t;

// win_info_t constructor. Creates new XWindow and allocates OpenGL context
win_info_t* w_create(
        uint16_t win_w, uint16_t win_h,
        uint16_t win_x, uint16_t win_y,
        const char* win_caption, bool verbose,
        bool use_double_buffer);

// Sets GLXContext options. Should be called creating win_info_t instance
void w_set_glx_context_version(int major, int minor, int flags);

// Destroys window and frees all its data, closes OpenGL context
void w_destroy(win_info_t* w);

// If double buffering enabled swaps window buffers
void w_swap_buffers(win_info_t* w);

// Prints some GLX information
void w_print_info();

#endif //DEEPCARS_WIN_H
