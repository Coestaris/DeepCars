//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_WIN_DEFAULTS_H
#define DEEPCARS_WIN_DEFAULTS_H

#include "../lib/coredefs.h"
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

//
// Map defines
//

#define MAP_MAIN_DEPTH 2
#define MAP_FLAGS_DEPTH 1
#define MAP_MAIN_LINE_WIDTH 1.5
#define MAP_MAIN_LINE_COLOR COLOR_WHITE

#define MAP_ROTATION_STEP 36
#define MAP_ROTATION_STEP_SMALL 72

#define MAP_FLAGS_MIN_DIST 50
#define MAP_FLAGS_MAX_DIST M_SQRT2 * (float)default_win->w / 4

#define MAP_FLAGS_LINE_WIDTH 1.1
#define MAP_FLAGS_LINE_COLOR COLOR_BLACK
#define MAP_FLAGS_ARROW_L 40
#define MAP_FLAGS_ARROW_W 4
#define MAP_FLAGS_ARROW_H 10
#define MAP_FLAGS_ARROW_COLOR COLOR_GREEN
#define MAP_DEFAULT_MAP "/1.map"

//
// Editor defines
//
#define EDITOR_GRID_COLOR ((oilColor){90, 90, 90, 220});
#define EDITOR_GRID_COLOR_ACCENT ((oilColor){120, 120, 120, 200});

#define EDITOR_MAIN_DEPTH 3
#define EDITOR_GRID_DEPTH 1

#define EDITOR_GRID_SIZE_0 0
#define EDITOR_GRID_SIZE_1 8
#define EDITOR_GRID_SIZE_2 16
#define EDITOR_GRID_SIZE_3 32
#define EDITOR_GRID_SIZE_4 128
#define EDITOR_GRID_START EDITOR_GRID_SIZE_3
#define EDITOR_GRID_START_STATE 2
#define EDITOR_GRID_STATES 6
#define EDITOR_DEFALUT_MAP_FN "map.map"

#define EDITOR_MENU_TABBUTTON_FFNN_POS 1168, 16
#define EDITOR_MENU_TABBUTTON_GA_POS 1168, 128
#define EDITOR_MENU_TABBUTTON_MAP_POS 1168, 240
#define EDITOR_MENU_TABBUTTON_FILE_POS 1168, 352
#define EDITOR_FILE_FILTER "Map files:map"

#define EDITOR_MENU_TAB_FILE_SAVE_POS 895, 400
#define EDITOR_MENU_TAB_FILE_LOAD_POS 1027, 400
#define EDITOR_MENU_TAB_POS 871, 7
#define EDITOR_MENU_RUNBTN_POS 895, 698
#define EDITOR_MENU_ERASER_POS 16, 704
#define EDITOR_MENU_GRID_POS 16, 752
#define EDITOR_MENU_WALL_POS 80, 752
#define EDITOR_MENU_START_POS 120, 752
#define EDITOR_MENU_FIN_POS 160, 752
#define EDITOR_MENU_OBSTACLE_POS 200, 752
#define EDITOR_MENU_SLIP_POS 240, 752
#define EDITOR_MENU_DEPTH 4

//
// Wall defines
//
#define WALL_HEIGHT 15
#define WALL_WIDTH 10

extern win_info_t* win;
extern camera_t* camera;

#endif //DEEPCARS_WIN_DEFAULTS_H
