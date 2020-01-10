//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_SCENE_H
#define DEEPCARS_SCENE_H

#include <stdint.h>

#include "structs.h"
#include "graphics/camera.h"
#include "graphics/rendering/graphics.h"
#include "graphics/light.h"
#include "object.h"

// Structure for ease movement between scenes (changing one set of objects, lights to a second)
typedef struct _scene
{
   uint32_t id;
   // Light set of this scene
   list_t* lights;

   // Set of objects that loads to an updater when scene is will be loading
   list_t* startup_objects;

   texture_t* skybox;

   shadow_light_t* shadow_light;

} scene_t;

// scene_t constructor
scene_t* sc_create(uint32_t id);

// Frees scene and all its resources
void sc_free(scene_t* scene);

#endif //DEEPCARS_SCENE_H
