//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/win.h"
#include "../lib/graphics/rendering/graphics.h"
#include "../lib/updater.h"
#include "../lib/scm.h"
#include "objects/obj_dummy.h"
#include "win_defaults.h"
#include "objects/obj_camera_control.h"
#include "../lib/resources/pack.h"
#include "../lib/graphics/rendering/render_chain.h"
#include "renderer.h"
#include "../lib/resources/rmanager.h"

win_info_t*    win;
mat4           view;
camera_t*      camera;

vec4 plane_color;
vec4 sky_color;

vec4 light_pos;
mat4 light_view;
mat4 light_proj;
mat4 light_space;
camera_t* light_camera;

void app_load_resources(void)
{
   // scenes
   scene_t* menu = sc_create(SCENEID_MENU);
   menu->back_color = COLOR_GRAY;
   // objects

   plane_color = cvec4(129 / 255.0f, 146 / 255.0f, 89 / 255.0f, 0);
   model_t* plane = m_create_plane(30, 30, true);
   m_normalize(plane, true, true, true, true);
   m_build(plane);

   rm_push(MODEL, plane, MODELID_PLANE);

   list_push(menu->startup_objects,
             create_textured_dummy(vec3f(-500, 0, -500), 1000,
                   rm_get(TEXTURE, 3),
                   rm_get(MODEL, MODELID_PLANE)));

   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-16, 0, -4), 10,
                     rm_get(TEXTURE, 0),
                     rm_get(MODEL, MODELID_CUBE)));

   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-4, 0, -16), 10,
                     rm_get(TEXTURE, 1),
                     rm_get(MODEL, MODELID_TORUS)));
   list_push(menu->startup_objects,
           create_textured_dummy(vec3f(-16, 0, -16), 10,
                     rm_get(TEXTURE, 2),
                     rm_get(MODEL, MODELID_TEAPOT)));

   list_push(menu->startup_objects, create_camera_control());

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

   if (VERBOSE)
      w_print_info();

   u_init();
   scm_init();
   s_init();
   rm_init();

   p_load(RESOURCE_PACK_FILE);

   gr_init();

   camera = c_create(
   cvec4(0, 5, 15, 0),
   cvec4(0, 1, 0, 0));

   rc_create_perspective(win, proj_mat, 65.f, 0.1f, 200);
   //ortho(proj_mat, -1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

   light_pos = cvec4(0, 20, 0, 0);

   light_proj = cmat4();
   light_view = cmat4();
   light_space = cmat4();

   float near_plane = 1.0f, far_plane = 100.0f;
   //rc_create_ortho(win, light_proj, near_plane, far_plane);
   mat4_ortho(light_proj, near_plane, far_plane, 50, 50);
   //ortho(light_proj, -1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

   light_camera = c_create(light_pos, camera->up);
   light_camera->use_target = true;
   light_camera->target = cvec4(-16, 0, -4, 0);
   c_to_mat(light_view, light_camera);


   rc_set_current(get_chain(win, camera, proj_mat));

   GL_PCALL(glEnable(GL_DEPTH_TEST));
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

   u_clear_objects(false);
   u_free();


   scm_free();
   gr_free();

   rm_free(true, true, true);
   s_free(true);

   w_destroy(win);
}