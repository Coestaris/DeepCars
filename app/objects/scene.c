//
// Created by maxim on 1/3/20.
//

#include "../../lib/scene.h"
#include "../../lib/resources/rmanager.h"
#include "obj_dummy.h"
#include "obj_camera_control.h"
#include "../win_defaults.h"
#include "obj_info_draw.h"
#include "../rendering/renderer.h"
#include "menu/obj_menu_drawer.h"
#include "menu/obj_menu_camera_mover.h"
#include "obj_default_bind_handler.h"

void setup_objects(scene_t* scene)
{
   model_t* plane = m_create_plane(50, 50, true);
   m_normalize(plane, true, true, true, true);
   m_build(plane);

   rm_push(MODEL, plane, -1);

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-1000, 0, -1000), 2000,
                                   rm_getn(MATERIAL, "grass"),
                                   plane));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(0,0,0), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_getn(MODEL, "cube")));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(0, 0, -16), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_getn(MODEL, "torus")));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(0, 0, 16), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_getn(MODEL, "torus")));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-16, 0, 0), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_getn(MODEL, "teapot")));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(16, 0, 0), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_getn(MODEL, "teapot")));

   const int count = 20;
   const float step = 2.0f * M_PI / count;
   for(int i = 0; i < count; i++)
   {
      object_t* obj =   create_textured_dummy(
            vec3f(cosf(i * step) * 70.0f, 0, sinf(i * step) * 70.0f),
            30,
            rm_getn(MATERIAL, "column"),
            rm_getn(MODEL, "column"));
      obj->rotation.y = i * step + M_PI / 2;

      list_push(scene->startup_objects, obj);
   }
   list_push(scene->startup_objects, create_default_bind_handler());
   list_push(scene->startup_objects, create_menu_camera_mover());
   //list_push(scene->startup_objects, create_camera_control());
   list_push(scene->startup_objects, create_menu_drawer());
   list_push(scene->startup_objects, create_info_drawer());
}

void setup_shadow_light(scene_t* scene)
{
   const float near_plane = 0.01f;
   const float far_plane = 200.0f;
   const float r = 1024 / 4.0f;
   const float t = 1024 / 4.0f;

   shadow_light_t* shadow_light = l_sh_create(cvec4(0, 0, 0, 0), vec4_ccpy(camera->up));
   mat4_ortho(shadow_light->light_proj, near_plane, far_plane, r, t);
   shadow_light->light_camera->use_target = true;
   shadow_light->light_camera->target = cvec4(0, 0, 0, 0);
   scene->shadow_light = shadow_light;

   shadow_light->brightness = 1.0f;
}

void setup_lights(scene_t* scene)
{
   const float angle = M_PI * 2 / NR_LIGHTS;
   const float radius = 70;

   const float linear = 0.12f;
   const float linear_rand = 0.0f;
   const float quadratic = 0.019f;
   const float quadratic_rand = 0.0f;

   for (size_t i = 0; i < NR_LIGHTS; i++)
   {
      light_t* lt = l_create();

      float x_pos = sinf(angle * i) * radius;
      float y_pos = 10;
      float z_pos = cosf(angle * i) * radius;
      vec4_fill(lt->position, x_pos, y_pos, z_pos, 0);

      float r_color = (float)drand48() * 0.5f + 0.5f;
      float g_color = (float)drand48() * 0.5f + 0.5f;
      float b_color = (float)drand48() * 0.5f + 0.5f;
      vec4_fill(lt->color, r_color, g_color, b_color, 0);

      lt->constant = 1.0f;
      lt->linear = linear + (float)drand48() * linear_rand;
      lt->quadratic = quadratic + (float)drand48() * quadratic_rand;

      l_calc_radius(lt);
      list_push(scene->lights, lt);
   }
}

void setup_light(scene_t* scene)
{
   setup_shadow_light(scene);
   setup_lights(scene);
}
