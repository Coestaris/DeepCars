//
// Created by maxim on 8/28/19.
//

#ifndef ZOMBOID3D_UPDATER_H
#define ZOMBOID3D_UPDATER_H

#include <stddef.h>
#include <sys/time.h>
#include <zconf.h>
#include <stdio.h>
#include <X11/Xlib.h>

#include "graphics/win.h"

#define FPSToLock 60.0
#define FPSDelay 1000.0 / FPSToLock
#define FPSAvCounter 1000.0

typedef void (*drawFunc_t)();
typedef void (*eventHandler_t)(XEvent event);

typedef enum _keyState {
    pressed,
    released
} keyState_t;

typedef enum _mouseState {
    pressedasd
} mouseState_t;


void u_close();
void u_startLoop(
        winInfo_t* info,
        drawFunc_t drawFunc,
        eventHandler_t eventHandler);


#endif //ZOMBOID3D_UPDATER_H
