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

#define KEY_RELEASE 0
#define KEY_PRESSED 1

#define MOUSE_RELEASE 0
#define MOUSE_PRESSED 1

#define MOUSE_LEFT 1
#define MOUSE_MIDDLE 2
#define MOUSE_RIGHT 3

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

vec2f_t u_getMousePos();
int u_getKeyState(int key);
int u_getMouseState(int mouse);

#endif //DEEPCARS_UPDATER_H
