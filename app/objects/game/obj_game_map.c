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
#include "../../rendering/vfx.h"

static bool first_map_load = true;

static vec2 get_point(float w, float h, float max)
{
   for(size_t i = 0; i < 200; i++)
   {
      float x = rand_range(-max, max);
      float y = rand_range(-max, max);

      if(fabsf(x) < w && fabs(y) < h)
         continue;

      return vec2f(x, y);
   }

   return vec2e;
}

static void push_collection(model_t* model, material_t* material, float size, size_t count)
{
   instance_collection_t* collection = ic_create(model, material, count);
   for(size_t i = 0; i < count; i++)
   {
      //float angle = rand_range(0, M_PI * 2);
      //float r = rand_range(130, 430);

      size += size * rand_range(-0.05f, 0.05f);
      vec2 pos = get_point(180, 110, 320);
      pos.x += 60;

      mat4 m = cmat4();
      gr_transform(vec3f(pos.x, surface_func(pos.x / 4000, pos.y / 4000) * 4000 - 0.5, pos.y),
                   vec3f(size, size, size),
                   vec3f(0, rand_range(0, M_PI * 2), 0));
      mat4_cpy(m, model_mat);
      ic_set_mat(collection, i, m);
   }

   ic_push(collection);
}

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

      //if(!found)
         //APP_ERROR("Unable to find start object",0);

      object_t* car = create_controllable_car(
            vec2f(start_pos.x * 0.3f - 150, start_pos.y * 0.3f - 100),
            start_rotation,
            camera);
      u_push_object(car);

      first_map_load = false;

      push_collection(rm_getn(MODEL, "grass1"), rm_getn(MATERIAL, "grass_dec"), 30, 50);
      push_collection(rm_getn(MODEL, "grass2"), rm_getn(MATERIAL, "grass_dec"), 30, 50);
      push_collection(rm_getn(MODEL, "grass3"), rm_getn(MATERIAL, "grass_dec"), 30, 50);
      push_collection(rm_getn(MODEL, "tree1"), rm_getn(MATERIAL, "tree1"), 25, 300);
      push_collection(rm_getn(MODEL, "tree2"), rm_getn(MATERIAL, "tree1"), 25, 200);

      push_collection(rm_getn(MODEL, "stone1"), rm_getn(MATERIAL, "column"), 5, 10);
      push_collection(rm_getn(MODEL, "stone2"), rm_getn(MATERIAL, "column"), 5, 10);
      push_collection(rm_getn(MODEL, "stone3"), rm_getn(MATERIAL, "column"), 5, 10);
      push_collection(rm_getn(MODEL, "stone4"), rm_getn(MATERIAL, "column"), 5, 10);
      push_collection(rm_getn(MODEL, "stone5"), rm_getn(MATERIAL, "column"), 5, 10);
   }
}

object_t* create_game_map()
{
   object_t* this = o_create();
   this->update_func = game_map_update;
   return this;
}


