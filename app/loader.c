//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/win.h"
#include "../lib/graphics/graphics.h"
#include "../lib/updater.h"
#include "../lib/scm.h"

#include "objects/obj_dummy.h"
#include "win_defaults.h"
#include "objects/obj_camera_control.h"

win_info_t*    win;
list_t*        models;
mat4           view;

model_t* get_model(uint32_t id)
{
   return models->collection[id];
}

model_t* load_and_build_model(const char* filename)
{
   model_t* model = m_load(filename);
   m_build(model);
   return model;
}

void app_load_resources(void)
{
   //models
   models = list_create(10);
   list_push(models, load_and_build_model("cube.obj"));
   list_push(models, load_and_build_model("torus.obj"));
   list_push(models, load_and_build_model("teapot.obj"));

   //scenes
   scene_t* menu = sc_create(SCENEID_MENU);
   list_push(menu->startup_objects, create_dummy(vec3f(6, 0, 12), .6f, COLOR_GREEN, get_model(MODELID_CUBE)));
   list_push(menu->startup_objects, create_dummy(vec3f(11, 0, 12), .03f, COLOR_GREEN, get_model(MODELID_TORUS)));
   list_push(menu->startup_objects, create_dummy(vec3f(6, 0, 6), .3f, COLOR_GREEN, get_model(MODELID_TEAPOT)));
   list_push(menu->startup_objects, create_camera_control());
   menu->camera = c_create(
           cvec4(0, 0, 15, 0),
           cvec4(0, 1, 0, 0));

   scm_push_scene(menu);
}

void app_init_graphics(void)
{
   win = w_create(
           WIN_WIDTH,
           WIN_HEIGHT,
           0, 0,
           WIN_TITLE,
           VERBOSE,
           stdout);

   const float angle_of_view = 60.0f;
   const float near = 0.1f;
   const float far = 200.0f;
   const float image_aspect_ratio = (float) win->w / (float) win->h;
   win->projection = cmat4();
   mat4_perspective_fov(win->projection, angle_of_view, image_aspect_ratio, near, far);

   view = cmat4();
   if (VERBOSE)
      w_print_info();

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