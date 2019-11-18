//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_SCENEMANAGER_H
#define DEEPCARS_SCENEMANAGER_H

#include "scene.h"
#include "updater.h"

void scm_init();
void scm_pushScene(scene_t* scene);
void scm_loadScene(uint32_t id, bool free);

#endif //DEEPCARS_SCENEMANAGER_H
