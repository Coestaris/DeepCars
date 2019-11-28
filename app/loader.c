//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/win.h"
#include "../lib/graphics/graphics.h"
#include "../lib/updater.h"
#include "../lib/scm.h"
#include "../lib/resources/txm.h"

#include "objects/obj_dummy.h"
#include "win_defaults.h"
#include "objects/obj_camera_control.h"
#include "../lib/resources/mm.h"
#include "../lib/resources/pack.h"

win_info_t*    win;
mat4           view;

vec4 plane_color;
vec4 sky_color;

void app_load_resources(void)
{
   p_load("resources.bin");

   // scenes
   scene_t* menu = sc_create(SCENEID_MENU);
   menu->back_color = COLOR_WHITE;
   // objects

   plane_color = cvec4(129 / 255.0f, 146 / 255.0f, 89 / 255.0f, 0);
/*
   list_push(menu->startup_objects,
             create_colored_dummy(vec3f(-500, 0, -500), 1000, plane_color, get_model(MODELID_PLANE)));
*/

   list_push(menu->startup_objects,
             create_colored_shaded_dummy(vec3f(16, 0, 4), 10, .3f, COLOR_GRAY, mm_get(MODELID_CUBE)));
   list_push(menu->startup_objects,
             create_colored_shaded_dummy(vec3f(4, 0, 16), 10, .0f, COLOR_GRAY, mm_get(MODELID_TORUS)));
   list_push(menu->startup_objects,
             create_colored_shaded_dummy(vec3f(16, 0, 16), 10, .3f, COLOR_GRAY, mm_get(MODELID_TEAPOT)));

   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-16, 0, -4), 10, txm_get(1), mm_get(MODELID_CUBE)));
   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-4, 0, -16), 10, txm_get(1), mm_get(MODELID_TORUS)));
   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-16, 0, -16), 10, txm_get(1), mm_get(MODELID_TEAPOT)));

   list_push(menu->startup_objects, create_camera_control());

   // camera
   menu->camera = c_create(
           cvec4(0, 5, 15, 0),
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
   txm_init();
   mm_init();

   gr_init(win->projection, view);
}

void app_run(void)
{
   scm_load_scene(SCENEID_MENU, true);
   u_start_loop(win);
}

void app_fin()
{
   vec4_free(plane_color);
   printf("[loader.c]: CLOSING....\n");

   u_clear_objects(true);
   u_free();


   scm_free();
   gr_free();

   txm_free(true);
   mm_free(true);
   s_free(true);

   w_destroy(win);
}