//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_LOADER_H
#define DEEPCARS_LOADER_H

#include "../lib/scene.h"

void setup_objects(scene_t* scene);
void setup_light(scene_t* scene);

void app_init_graphics(void);
void app_load_resources(void);
void app_run(void);

void app_fin(void);

#endif //DEEPCARS_LOADER_H
