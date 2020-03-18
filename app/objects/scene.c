//
// Created by maxim on 1/3/20.
//

#include "../../lib/scene.h"
#include "../../lib/resources/rmanager.h"
#include "../rendering/renderer.h"
#include "../win_defaults.h"
#include "obj_dummy.h"
#include "obj_info_draw.h"
#include "obj_default_bind_handler.h"
#include "menu/obj_menu_drawer.h"
#include "menu/obj_menu_camera_mover.h"
#include "menu/obj_menu_phys.h"
#include "editor/obj_editor_drawer.h"
#include "editor/obj_editor_map.h"
#include "obj_camera_control.h"
#include "game/obj_game_map.h"

model_t* plane;

void setup_editor_objects(scene_t* scene)
{
   scene->backcolor = cvec4(0.50, 0.56640625, 0.34765625, 0);
   list_push(scene->startup_objects, create_editor_drawer());
   list_push(scene->startup_objects, create_editor_map());
}

void setup_game_objects(scene_t* scene)
{
   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-1000, 0, -1000), 2000,
                                   rm_getn(MATERIAL, "grass"),
                                   plane));
   //list_push(scene->startup_objects, create_camera_control());
   list_push(scene->startup_objects, create_default_bind_handler());
   list_push(scene->startup_objects, create_game_map());
}

static void push_collection(model_t* model, material_t* material, float size, size_t count)
{
   instance_collection_t* collection = ic_create(model, material, count);
   for(size_t i = 0; i < count; i++)
   {
      float angle = drand48() * M_PI * 2;
      float r = drand48() * 350 + 70 + 10;

      float x = cosf(angle)  * r;
      float y = sinf(angle)  * r;

      mat4 m = cmat4();
      gr_transform(vec3f(x, 0, y),
                   vec3f(size, size, size),
                   vec3f(0, drand48() * M_PI * 2, 0));
      mat4_cpy(m, model_mat);
      ic_set_mat(collection, i, m);
   }

   ic_push(collection);
}

void setup_menu_objects(scene_t* scene)
{
   if(!plane)
   {
      plane = m_create_plane(150, 150, true);
      m_normalize(plane, true, true, true, true);
      m_build(plane);
      rm_push(MODEL, plane, -1);
   }

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-1000, 0, -1000), 2000,
                                   rm_getn(MATERIAL, "grass"),
                                   plane));

   const size_t count = 20;
   const float step = 2.0f * M_PI / count;
   const float radius = 70;
   for(size_t i = 0; i < count; i++)
   {
      object_t* obj = create_textured_dummy(
            vec3f(cosf(i * step) * radius, 0, sinf(i * step) * radius),
            30,
            rm_getn(MATERIAL, "column"),
            rm_getn(MODEL, "column"));
      obj->rotation.y = i * step + M_PI / 2;

      list_push(scene->startup_objects, obj);
   }

   push_collection(rm_getn(MODEL, "grass1"), rm_getn(MATERIAL, "grass_dec"), 30, 50);
   push_collection(rm_getn(MODEL, "grass2"), rm_getn(MATERIAL, "grass_dec"), 30, 50);
   push_collection(rm_getn(MODEL, "grass3"), rm_getn(MATERIAL, "grass_dec"), 30, 50);
   push_collection(rm_getn(MODEL, "tree1"), rm_getn(MATERIAL, "tree1"), 25, 200);
   push_collection(rm_getn(MODEL, "tree2"), rm_getn(MATERIAL, "tree1"), 30, 100);

   push_collection(rm_getn(MODEL, "stone1"), rm_getn(MATERIAL, "column"), 5, 10);
   push_collection(rm_getn(MODEL, "stone2"), rm_getn(MATERIAL, "column"), 5, 10);
   push_collection(rm_getn(MODEL, "stone3"), rm_getn(MATERIAL, "column"), 5, 10);
   push_collection(rm_getn(MODEL, "stone4"), rm_getn(MATERIAL, "column"), 5, 10);
   push_collection(rm_getn(MODEL, "stone5"), rm_getn(MATERIAL, "column"), 5, 10);

   for(size_t i = 0; i < SPHERES_COUNT; i++)
   {
      render_spheres[i] = create_textured_dummy(vec3f(0,0,0), SPHERE_RADIUS,
            rm_getn(MATERIAL, "menu_spheres"),
            rm_getn(MODEL, "sphere"));
   }
   render_car1 = create_textured_dummy(vec3f(0,0,0), CAR_SIZE,
            rm_getn(MATERIAL, "car1"),
            rm_getn(MODEL, "car"));

   render_car2 = create_textured_dummy(vec3f(0,0,0), CAR_SIZE,
            rm_getn(MATERIAL, "car2"),
            rm_getn(MODEL, "car"));

   object_t* rng = NULL;
   list_push(scene->startup_objects, rng = create_textured_dummy(vec3f(0,0,0), 140,
             rm_getn(MATERIAL, "column"),
             rm_getn(MODEL, "ring")));
   rng->scale.y = 155;

   list_push(scene->startup_objects, rng = create_textured_dummy(vec3f(0,0,0), 26,
                                                           rm_getn(MATERIAL, "column"),
                                                           rm_getn(MODEL, "ring")));
   rng->scale.y = 300;

   list_push(scene->startup_objects, create_textured_dummy(vec3f(-1.2,0,1.2), 60,
                                                           rm_getn(MATERIAL, "column"),
                                                           rm_getn(MODEL, "lenin")));

   list_push(scene->startup_objects, create_default_bind_handler());
   list_push(scene->startup_objects, create_menu_camera_mover());
   //list_push(scene->startup_objects, create_camera_control());
   list_push(scene->startup_objects, create_menu_drawer());
   list_push(scene->startup_objects, create_menu_phys());
   //list_push(scene->startup_objects, create_info_drawer());
}

void setup_menu_shadow_light(scene_t* scene)
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

   shadow_light->brightness = 0.8f;
}

void setup_menu_lights(scene_t* scene)
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
