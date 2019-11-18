//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/win.h"
#include "../lib/graphics/graphics.h"
#include "../lib/updater.h"
#include "../lib/shaders/shaderMgr.h"
#include "../lib/sceneManager.h"

#include "objects/obj_cube.h"
#include "winDefaults.h"
#include "objects/obj_camera_control.h"

winInfo_t* win;
list_t* models;
mat4 view;

model_t* getModel(uint32_t id)
{
    return models->collection[id];
}

model_t* loadAndBuildModel(const char* filename)
{
    model_t* model = m_load(filename);
    m_build(model);
    return model;
}

void app_loadResources(void)
{
    //models
    models = createList(10);
    listPush(models, loadAndBuildModel("cube.obj"));

    //scenes
    scene_t* menu = sc_create(SCENEID_MENU);
    listPush(menu->startupObjects, create_cube(vec3f(0, 0, 0), 5, NULL));
    listPush(menu->startupObjects, create_cube(vec3f(-5, 0, 0), 6, NULL));
    listPush(menu->startupObjects, create_cube(vec3f(0, 5, 0), 7, NULL));
    listPush(menu->startupObjects, create_cameraControl());

    scm_pushScene(menu);
}

void app_initGraphics(void)
{
    win = w_create(
            WIN_WIDTH,
            WIN_HEIGHT,
            0, 0,
            WIN_TITLE,
            VERBOSE,
            stdout);

    const float angleOfView = 60.0f;
    const float near = 0.1f;
    const float far = 100.0f;
    const float imageAspectRatio = (float)win->w / (float)win->h;
    win->projection = cmat4();
    perspectiveFovMat(win->projection, angleOfView, imageAspectRatio, near, far);

    view = cmat4();
    if(VERBOSE)
        w_printInfo();

    u_init();
    scm_init();
    s_init();
    gr_init(win->projection, view);
}

void app_run(void)
{
    scm_loadScene(SCENEID_MENU, true);
    u_startLoop(win);
}

void app_fin()
{
    s_free();

    u_clearObjects(true);
    u_free();

    for(size_t i = 0; i < models->count; i++)
        m_free((model_t*)models->collection[i]);
    listFree(models);

    scm_free();
    gr_free();

    w_destroy(win);
}