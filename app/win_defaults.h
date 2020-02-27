//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_WIN_DEFAULTS_H
#define DEEPCARS_WIN_DEFAULTS_H

#include "../lib/graphics/win.h"

#define APP_LOG(format, ...) DC_LOG("app", format, __VA_ARGS__)
#define APP_ERROR(format, ...) DC_ERROR("app", format, __VA_ARGS__)

#define RESOURCE_PACK_FILE "resources.bin"

#define NR_LIGHTS 30
#define KERNEL_SIZE 64

#define WIN_WIDTH  1200
#define WIN_HEIGHT 800
#define WIN_TITLE  "DeepCars"

#define SCENEID_MENU 0
#define SCENEID_EDITOR 1
#define SCENEID_GAME 2

extern win_info_t* win;
extern camera_t* camera;

#endif //DEEPCARS_WIN_DEFAULTS_H
