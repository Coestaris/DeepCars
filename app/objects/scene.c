//
// Created by maxim on 1/3/20.
//

#include "../../lib/scene.h"
#include "../../lib/resources/rmanager.h"
#include "obj_dummy.h"
#include "obj_camera_control.h"
#include "../win_defaults.h"

void setup_objects(scene_t* scene)
{
   model_t* plane = m_create_plane(30, 30, true);
   m_normalize(plane, true, true, true, true);
   m_build(plane);

   rm_push(MODEL, plane, MODELID_PLANE);

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-500, 0, -500), 1000,
                                   rm_getn(MATERIAL, "grass"),
                                   rm_get(MODEL, MODELID_PLANE)));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-16, 0, -4), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_get(MODEL, MODELID_CUBE)));

   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-4, 0, -16), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_get(MODEL, MODELID_TORUS)));
   list_push(scene->startup_objects,
             create_textured_dummy(vec3f(-16, 0, -16), 10,
                                   rm_getn(MATERIAL, "default"),
                                   rm_get(MODEL, MODELID_TEAPOT)));

   list_push(scene->startup_objects, create_camera_control());
}

void setup_light(scene_t* scene)
{
/*   scene->light = l_create();
   scene->light->position = cvec4(0, 20, 0, 0);
   vec4_fill(scene->light->color, 0.4, 0.4, 0.4, 0);

   float near_plane = 1.0f, far_plane = 100.0f;
   mat4_ortho(scene->light->light_proj, near_plane, far_plane, 50, 50);

   scene->light->light_camera = c_create(vec4_ccpy(scene->light->position), vec4_ccpy(camera->up));
   scene->light->light_camera->use_target = true;
   scene->light->light_camera->target = cvec4(-16, 0, -4, 0);
   list_push(scene->lights, scene->light);*/

   for (size_t i = 0; i < NR_LIGHTS; i++)
   {
      light_t* lt = l_create();

      // calculate slightly random offsets
      float xPos = ((float)(rand() % 100) / 100.0f) * 6.0f - 3.0f;
      float yPos = ((float)(rand() % 100) / 100.0f) * 6.0f - 4.0f;
      float zPos = ((float)(rand() % 100) / 100.0f) * 6.0f - 3.0f;
      vec4_fill(lt->position, xPos, yPos, zPos, 0);

      float rColor = ((float)(rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
      float gColor = ((float)(rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
      float bColor = ((float)(rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
      vec4_fill(lt->color, rColor, gColor, bColor, 0);

      list_push(scene->lights, lt);
   }

}
