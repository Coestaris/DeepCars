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
   models = list_create(10);
   list_push(models, loadAndBuildModel("teapot.obj"));

   //scenes
   scene_t* menu = sc_create(SCENEID_MENU);
   list_push(menu->startup_objects, create_cube(vec3f(5, 5, 0), .3f, COLOR_GREEN));
   //list_push(menu->startup_objects, create_cube(vec3f(-5, 5, 0), 3, COLOR_BLUE));
   //list_push(menu->startup_objects, create_cube(vec3f(5, -5, 0), 1, COLOR_RED));
   list_push(menu->startup_objects, create_cameraControl());

   scm_push_scene(menu);
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
   const float far = 200.0f;
   const float imageAspectRatio = (float) win->w / (float) win->h;
   win->projection = cmat4();
   perspectiveFovMat(win->projection, angleOfView, imageAspectRatio, near, far);

   view = cmat4();
   if (VERBOSE)
      w_printInfo();

   u_init();
   scm_init();
   s_init();
   gr_init(win->projection, view);
}

void app_run(void)
{
   scm_load_scene(SCENEID_MENU, true);
   u_start_loop(win);
}

void app_fin()
{
   s_free();

   u_clear_objects(true);
   u_free();

   for (size_t i = 0; i < models->count; i++)
      m_free((model_t*) models->collection[i]);
   list_free(models);

   scm_free();
   gr_free();

   w_destroy(win);
}