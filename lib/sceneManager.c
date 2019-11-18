//
// Created by maxim on 11/18/19.
//

#include "sceneManager.h"

list_t* scenes;
int currentSceneID = -1;

scene_t* scm_getCurrent(void)
{
    return (scene_t*)scenes->collection[currentSceneID];
}

void scm_init()
{
    scenes = createList(10);
}

void scm_pushScene(scene_t* scene)
{
    listPush(scenes, scene);
}

void scm_loadScene(uint32_t id, bool free)
{
    if(currentSceneID != -1)
    {
        //todo
    }

    currentSceneID = id;
    u_clearObjects(free);
    scene_t* scene = (scene_t*)scenes->collection[id];

    for(size_t i = 0; i < scene->startupObjects->count; i++)
        u_pushObject(o_clone((drawableObject_t*)scene->startupObjects->collection[i]));
}

void scm_free()
{
    for(size_t i = 0; i < scenes->count; i++)
        sc_free((scene_t*)scenes->collection[i]);
    listFree(scenes);
}