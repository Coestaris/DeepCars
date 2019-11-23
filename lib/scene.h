//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_SCENE_H
#define DEEPCARS_SCENE_H

#include <stdint.h>

#include "structs.h"
#include "graphics/camera.h"
#include "graphics/graphics.h"
#include "graphics/light.h"
#include "object.h"

// Structure for ease movement between scenes (changing one set of objects, lights to a second)
typedef struct _scene
{
   uint32_t id;
   // Light set of this scene
   list_t* lights;
   // Camera to use in this scene
   camera_t* camera;
   // Set of objects that loads to an updater when scene is will be loading
   list_t* startup_objects;

   // Lists of texture and model scopes that should be loaded when
   // loading this scene. Scopes will be loaded by the respective managers.
   // Algorithm:
   //  1. Unload all loaded scopes that aren't needed by current scene
   //  2. Load all unloaded scopes that are needed by current scene
   list_t* required_tex_scopes;
   list_t* required_model_scopes;

   //todo: delete
   vec4 back_color;

} scene_t;

// scene_t constructor
scene_t* sc_create(uint32_t id);

// Frees scene and all its resources
void sc_free(scene_t* scene);

#endif //DEEPCARS_SCENE_H
