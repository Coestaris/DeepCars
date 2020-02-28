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

bool first_map_load = true;

void game_map_update(object_t* this)
{
   if(first_map_load)
   {
      model_t* m = build_map_model(walls);
      m_build(m);
      m_normalize(m, true, true, false, false);
      rm_push(MODEL, m, -1);

      u_push_object(create_textured_dummy(
            vec3f(0, 0, 0),
            0.3, rm_getn(MATERIAL, "menu_spheres"), m));

      first_map_load = false;
   }
}

object_t* create_game_map()
{
   object_t* this = o_create();
   this->update_func = game_map_update;
   return this;
}


