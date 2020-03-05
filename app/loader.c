//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/rendering/graphics.h"
#include "../lib/updater.h"
#include "../lib/scm.h"
#include "win_defaults.h"
#include "../lib/resources/pack.h"
#include "../lib/graphics/rendering/render_chain.h"
#include "rendering/renderer.h"
#include "../lib/resources/rmanager.h"
#include "rendering/text_rendering.h"

win_info_t*    win;
mat4           view;
camera_t*      camera;

void app_load_resources(void)
{
   scene_t* menu = sc_create(SCENEID_MENU);
   menu->skybox = rm_getn(TEXTURE, "skybox");
   setup_menu_objects(menu);
   setup_menu_lights(menu);
   setup_menu_shadow_light(menu);

   scene_t* editor = sc_create(SCENEID_EDITOR);
   setup_editor_objects(editor);

   scene_t* game = sc_create(SCENEID_GAME);
   game->skybox = rm_getn(TEXTURE, "skybox");
   setup_menu_shadow_light(game);
   setup_game_objects(game);

   scm_push_scene(menu);
   scm_push_scene(editor);
   scm_push_scene(game);
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
   mt_init();

   p_load(RESOURCE_PACK_FILE);

   gr_init();

   camera = c_create(
         cvec4(0, 45, 100, 0),
         cvec4(0, 1, 0, 0));

   rc_create_perspective(win, proj_mat, 65.f, 0.1f, 500);
   rc_set_current(get_chain(win, camera, proj_mat));
}

void app_run(void)
{
   scm_load_scene(SCENEID_MENU, true);

   init_fonts(win);

   switch_stages();
   switch_ssao();
   update_lights();
   update_shadow_light();

   u_start_loop(win);
}

void app_fin()
{
   APP_LOG("Closing....",0);

   u_clear_objects(false);
   u_release();

   mt_release();
   scm_release();
   gr_release();

   free_stages();

   rm_release(true, true, true, true);
   s_release(true);

   w_destroy(win);
}