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
#include "../editor/map_saver.h"
#include "obj_controllable_car.h"
#include "../../rendering/renderer.h"

static bool first_map_load = true;

static void game_map_update(object_t* this)
{
   if(first_map_load)
   {
      model_t* m = build_map_model(walls);
      m_build(m);
      //m_normalize(m, true, true, false, false);
      rm_push(MODEL, m, -1);

      object_t* obj = create_textured_dummy(
            vec3f(-150, 0, -100),
            0.3f, rm_getn(MATERIAL, "wall"), m);

      o_enable_draw_normals(obj, COLOR_GREEN, COLOR_WHITE, 15);
      u_push_object(obj);

      vec2 start_pos = vec2e;
      float start_rotation = 0;
      bool found = false;
      for(size_t i = 0; i < map_objects->count; i++)
      {
         map_object_t* object = map_objects->collection[i];
         if(object->type == START)
         {
            start_pos = object->pos;
            start_rotation = object->rotation + M_PI / 2.0;
            found = true;
            break;
         }
      }

      if(!found)
         APP_ERROR("Unable to find start object",0);

      object_t* car = create_controllable_car(
            vec2f(start_pos.x * 0.3f - 150, start_pos.y * 0.3f - 100),
            start_rotation,
            camera);
      u_push_object(car);

      first_map_load = false;
   }
}

object_t* create_game_map()
{
   object_t* this = o_create();
   this->update_func = game_map_update;
   return this;
}


