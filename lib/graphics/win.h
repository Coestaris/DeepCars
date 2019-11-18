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

typedef struct _winInfo {

    Display* display;

    Window win;
    GLXContext context;
    Colormap colorMap;

    uint16_t w;
    uint16_t h;

    const char* caption;

    uint16_t lockedFps;
    double lastFrame;
    mat4 projection;

} winInfo_t;

#define GLX_DEFAULT_MAJOR_VERSION 3
#define GLX_DEFAULT_MINOR_VERSION 0
#define GLX_DEFAULT_FLAGS         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#define DEFAULT_LOCKED_FPS        60

winInfo_t* w_create(uint16_t winW, uint16_t winH, uint16_t winX, uint16_t winY,
                    const char* winCaption, bool verbose, FILE* vf);
void w_setGLXContextVersion(int major, int minor, int flags);
void w_destroy(winInfo_t* w);
void w_swapBuffers(winInfo_t* w);
void w_printInfo();

#endif //DEEPCARS_WIN_H
