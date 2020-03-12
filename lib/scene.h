//
// Created by maxim on 11/17/19.
//

#ifndef DEEPCARS_SCENE_H
#define DEEPCARS_SCENE_H

#include "coredefs.h"

#include "graphics/camera.h"
#include "graphics/rendering/graphics.h"
#include "graphics/light.h"
#include "object.h"

// Structure for ease movement between scenes (changing one set of objects, lights to a second)
typedef struct _scene
{
   // Unique ID of current scene
   uint32_t id;

   // List of all lights presents in this scene
   list_t* lights;

   // Set of objects that will be loaded to the updater when scene is loaded
   list_t* startup_objects;

   // Skybox texture, if set to NULL ignored
   texture_t* skybox;

   // Shadow casting directional light, if set to NULL ignored
   shadow_light_t* shadow_light;

   // Background color, if set to NULL ignored
   vec4 backcolor;

} scene_t;

// scene_t constructor
scene_t* sc_create(uint32_t id);

// Frees scene and all its resources
void sc_free(scene_t* scene);

#endif //DEEPCARS_SCENE_H
