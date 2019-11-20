//
// Created by maxim on 8/26/19.
//

#ifndef DEEPCARS_WIN_H
#define DEEPCARS_WIN_H

#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdbool.h>
#include "gmath.h"
#include "camera.h"

typedef struct _win_info
{

   Display* display;

   Window win;
   GLXContext context;
   Colormap color_map;

   uint16_t w;
   uint16_t h;

   const char* caption;

   uint16_t locked_fps;
   double_t last_frame;
   mat4 projection;

} win_info_t;

#define GLX_DEFAULT_MAJOR_VERSION 3
#define GLX_DEFAULT_MINOR_VERSION 0
#define GLX_DEFAULT_FLAGS         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#define DEFAULT_LOCKED_FPS        60

win_info_t* w_create(uint16_t win_w, uint16_t win_h, uint16_t win_x, uint16_t win_y,
                     const char* win_caption, bool verbose, FILE* vf);

void w_set_glx_context_version(int major, int minor, int flags);
void w_destroy(win_info_t* w);
void w_swap_buffers(win_info_t* w);
void w_print_info();

#endif //DEEPCARS_WIN_H
