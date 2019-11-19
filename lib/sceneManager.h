//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_SCENEMANAGER_H
#define DEEPCARS_SCENEMANAGER_H

#include "scene.h"
#include "updater.h"

// Global current scene set by scm_load_scene
extern scene_t* current_scene;

// Init all private variables of scene manager
void scm_init(void);

// Push new scene to a manager's list
void scm_push_scene(scene_t* scene);

// Load scene and initialize all its startup objects
void scm_load_scene(uint32_t id, bool free);

// Free all used by scene manager resources
void scm_free(void);

// Get loaded scene
scene_t* scm_get_current(void);

#endif //DEEPCARS_SCENEMANAGER_H
