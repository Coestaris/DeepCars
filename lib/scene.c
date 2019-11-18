//
// Created by maxim on 11/17/19.
//

#include "scene.h"

scene_t* sc_create(uint32_t id)
{
    scene_t* scene = malloc(sizeof(scene_t));
    scene->id = id;
    scene->camera = c_create(
            cvec4(0, 0, 0, 0),
            cvec4(0, 1, 0, 0));

    scene->lights = createList(100);
    scene->startupObjects = createList(100);

    return scene;
}

void sc_free(scene_t* scene)
{
    c_free(scene->camera);
    for(size_t i = 0; i < scene->lights->count; i++)
        l_free((light_t*)scene->lights->collection[i]);

    for(size_t i = 0; i < scene->startupObjects->count; i++)
        o_free((drawableObject_t*)scene->startupObjects->collection[i]);

    listFree(scene->lights);
    listFree(scene->startupObjects);

    free(scene);
}