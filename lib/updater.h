//
// Created by maxim on 8/28/19.
//

#ifndef DEEPCARS_UPDATER_H
#define DEEPCARS_UPDATER_H

#include <stddef.h>
#include <sys/time.h>
#include <zconf.h>
#include <stdio.h>
#include <X11/Xlib.h>

#include "graphics/win.h"
#include "drawableObject.h"

#define FPSToLock 60.0
#define FPSDelay 1000.0 / FPSToLock
#define FPSAvCounter 1000.0

void u_init(void);
void u_close(void);
void u_startLoop(winInfo_t* info);
void u_clearObjects(bool free);
void u_free();
void u_pushObject(drawableObject_t*object);
uint64_t u_getFrames(void);

#endif //DEEPCARS_UPDATER_H
