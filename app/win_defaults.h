//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_WIN_DEFAULTS_H
#define DEEPCARS_WIN_DEFAULTS_H

#define WIN_WIDTH  1200
#define WIN_HEIGHT 800
#define WIN_TITLE  "DeepCars"
#define VERBOSE    true

#define SCENEID_MENU 0

#define MODELID_CUBE 0

extern win_info_t* win;
extern list_t* models;
extern mat4 view;

model_t* get_model(uint32_t id);
model_t* get_material(uint32_t id);

#endif //DEEPCARS_WIN_DEFAULTS_H
