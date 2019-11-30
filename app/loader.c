//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/win.h"
#include "../lib/graphics/rendering/graphics.h"
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
   // scenes
   scene_t* menu = sc_create(SCENEID_MENU);
   menu->back_color = COLOR_WHITE;
   // objects

   plane_color = cvec4(129 / 255.0f, 146 / 255.0f, 89 / 255.0f, 0);
   model_t* plane = m_create_plane();
   m_normalize(plane, true, true, true, true);
   mm_push(MODELID_PLANE, plane, true);

   list_push(menu->startup_objects,
             create_colored_dummy(vec3f(-500, 0, -500), 1000, plane_color, mm_get(MODELID_PLANE)));

   list_push(menu->startup_objects,
             create_colored_shaded_dummy(vec3f(16, 0, 4), 10, .01f, COLOR_GRAY, mm_get(MODELID_CUBE)));
   list_push(menu->startup_objects,
             create_colored_shaded_dummy(vec3f(4, 0, 16), 10, .01f, COLOR_GRAY, mm_get(MODELID_TORUS)));
   list_push(menu->startup_objects,
             create_colored_shaded_dummy(vec3f(16, 0, 16), 10, .01f, COLOR_GRAY, mm_get(MODELID_TEAPOT)));

   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-16, 0, -4), 10, txm_get(0), mm_get(MODELID_CUBE)));
   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-4, 0, -16), 10, txm_get(1), mm_get(MODELID_TORUS)));
   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-16, 0, -16), 10, txm_get(2), mm_get(MODELID_TEAPOT)));

   list_push(menu->startup_objects, create_camera_control());

   // camera
   menu->camera = c_create(
           cvec4(0, 5, 15, 0),
           cvec4(0, 1, 0, 0));

   light_t* direction = l_create(LT_DIRECTION);
   vec4_cpy(direction->color, COLOR_WHITE);
   vec4_fill(direction->direction, 0, -1, 0.5, 0);
   list_push(menu->lights, direction);

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

   p_load(RESOURCE_PACK_FILE);
   s_setup_built_in_shaders();

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
   APP_LOG("Closing....",0);

   u_clear_objects(true);
   u_free();


   scm_free();
   gr_free();

   txm_free(true);
   mm_free(true);
   s_free(true);

   w_destroy(win);
}