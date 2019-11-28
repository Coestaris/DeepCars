//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_WIN_DEFAULTS_H
#define DEEPCARS_WIN_DEFAULTS_H

#define WIN_WIDTH  1200
#define WIN_HEIGHT 800
#define WIN_TITLE  "DeepCars"

#define SCENEID_MENU 0

#define MODELID_CUBE 0
#define MODELID_TORUS 1
#define MODELID_TEAPOT 2
#define MODELID_PLANE 3

extern win_info_t* win;
extern list_t* models;
extern mat4 view;

#endif //DEEPCARS_WIN_DEFAULTS_H
