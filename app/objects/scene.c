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