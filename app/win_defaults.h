//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_WIN_DEFAULTS_H
#define DEEPCARS_WIN_DEFAULTS_H

#define APP_LOG(format, ...) DC_LOG("app", format, __VA_ARGS__)
#define APP_ERROR(format, ...) DC_ERROR("app", format, __VA_ARGS__)

#define RESOURCE_PACK_FILE "resources.bin"

#define NR_LIGHTS 8
#define KERNEL_SIZE 64

#define WIN_WIDTH  1200
#define WIN_HEIGHT 800
#define WIN_TITLE  "DeepCars"

#define SCENEID_MENU 0

#define MODELID_CUBE 0
#define MODELID_TORUS 1
#define MODELID_TEAPOT 2
#define MODELID_PLANE 3

extern win_info_t* win;
extern mat4 view;
extern camera_t* camera;

#endif //DEEPCARS_WIN_DEFAULTS_H
