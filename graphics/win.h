//
// Created by maxim on 8/26/19.
//

#ifndef ZOMBOID3D_WIN_H
#define ZOMBOID3D_WIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdbool.h>

typedef struct _winInfo {

    Display* display;

    Window win;
    GLXContext context;
    Colormap colorMap;

    uint16_t w;
    uint16_t h;

    char* caption;

} winInfo_t;

#define GLX_DEFAULT_MAJOR_VERSION 3
#define GLX_DEFAULT_MINOR_VERSION 0
#define GLX_DEFAULT_FLAGS         GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB

winInfo_t* w_create(uint16_t winW, uint16_t winH, uint16_t winX, uint16_t winY,
                    char* winCaption, bool verbose, FILE* vf);
void w_setGLXContextVersion(int major, int minor, int flags);
void w_destroy(winInfo_t* w);
void w_swapBuffers(winInfo_t* w);

#endif //ZOMBOID3D_WIN_H
