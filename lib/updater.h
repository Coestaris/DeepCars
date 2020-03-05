//
// Created by maxim on 8/28/19.
//

#ifndef DEEPCARS_UPDATER_H
#define DEEPCARS_UPDATER_H

#include "coredefs.h"

#include <sys/time.h>
#include <zconf.h>
#include <X11/Xlib.h>

#include "graphics/win.h"
#include "object.h"

#define KEY_RELEASE 0
#define KEY_PRESSED 1

#define MOUSE_RELEASE 0
#define MOUSE_PRESSED 1

#define MOUSE_LEFT 1
#define MOUSE_MIDDLE 2
#define MOUSE_RIGHT 3
#define MOUSE_WHEELUP 4
#define MOUSE_WHEELDOWN 5

// Average FPS count to be locked
#define FPS_TO_LOCK 60.0
#define FPS_DELAY 1000.0 / FPS_TO_LOCK
// Period of measuring FPS
#define FPS_AV_COUNTER 1000.0

extern win_info_t* default_win;

// End application loop
void u_close(void);

// Initialize all private variables
void u_init(void);

// Start application loop
void u_start_loop(win_info_t* info);

// Clears all drawable objects
void u_clear_objects(bool free);

// Finalize all used resources
void u_release();

// Add specified object to global objects list
void u_push_object(object_t* object);

// Get current frame index
uint64_t u_get_frames(void);

// Get current mouse position
vec2 u_get_mouse_pos(void);

// Get current frames per second
double_t u_get_fps(void);

// Get state of specified keyboard key
int u_get_key_state(int key);

// Get state of mouse button
int u_get_mouse_state(int mouse);

// Get list of all objects
list_t* u_get_objects(void);

#endif //DEEPCARS_UPDATER_H
