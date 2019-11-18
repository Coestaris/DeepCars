//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_SCENEMANAGER_H
#define DEEPCARS_SCENEMANAGER_H

#include "scene.h"
#include "updater.h"

void scm_init(void);
void scm_pushScene(scene_t* scene);
void scm_loadScene(uint32_t id, bool free);
void scm_free(void);
scene_t* scm_getCurrent(void);

#endif //DEEPCARS_SCENEMANAGER_H
