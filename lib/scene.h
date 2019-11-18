//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_SCENE_H
#define DEEPCARS_SCENE_H

#include <stdint.h>

#include "structs.h"
#include "graphics/camera.h"
#include "graphics/light.h"
#include "drawableObject.h"

typedef struct _scene {
    uint32_t id;
    list_t* lights;
    camera_t* camera;
    list_t* startupObjects;

} scene_t;

scene_t* sc_create(uint32_t id);
void sc_free(scene_t* scene);

#endif //DEEPCARS_SCENE_H
