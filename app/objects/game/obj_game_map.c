//
// Created by maxim on 2/28/20.
//

#ifdef __GNUC__
#pragma implementation "obj_game_map.h"
#endif
#include "obj_game_map.h"
#include "../obj_dummy.h"
#include "map_builder.h"
#include "../editor/obj_editor_map.h"

static bool first_map_load = true;

void game_map_update(object_t* this)
{
   if(first_map_load)
   {
      model_t* m = build_map_model(walls);
      m_build(m);
      //m_normalize(m, true, true, false, false);
      rm_push(MODEL, m, -1);

      object_t* obj = create_textured_dummy(
            vec3f(-150, 0, 0),
            0.3f, rm_getn(MATERIAL, "menu_spheres"), m);

      o_enable_draw_normals(obj, COLOR_GREEN, COLOR_WHITE, 15);
      u_push_object(obj);

      first_map_load = false;
   }
}

object_t* create_game_map()
{
   object_t* this = o_create();
   this->update_func = game_map_update;
   return this;
}


