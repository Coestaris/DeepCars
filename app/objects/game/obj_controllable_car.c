//
// Created by maxim on 3/14/20.
//

#ifdef __GNUC__
#pragma implementation "obj_controllable_car.h"
#endif
#include "obj_controllable_car.h"
#include "../../rendering/renderer.h"
#include "car.h"
#include "../obj_dummy.h"

#define MAX_WHEEL_ANGLE M_PI / 9

static camera_t* car_camera;
static vec4 car_start = NULL;
static car_t* car = NULL;

static object_t* my_wheels[4];
static float wheel_angle = 0;

static void update_controllable_car(object_t* this)
{
   float delta = (float)u_get_delta();

   if(u_get_key_state(113) == KEY_PRESSED)
   {
      if (fabs(wheel_angle) < MAX_WHEEL_ANGLE)
         wheel_angle -= 0.01;
   }
   else if(u_get_key_state(114) == KEY_PRESSED)
   {
      if (fabs(wheel_angle) < MAX_WHEEL_ANGLE)
         wheel_angle += 0.01;
   }
   else
      wheel_angle *= 0.001;

      bool inputs[4];
   inputs[CAR_INPUT_F] = u_get_key_state(111) == KEY_PRESSED;
   inputs[CAR_INPUT_B] = u_get_key_state(116) == KEY_PRESSED;
   inputs[CAR_INPUT_R] = u_get_key_state(113) == KEY_PRESSED;
   inputs[CAR_INPUT_L] = u_get_key_state(114) == KEY_PRESSED;
   car_update(inputs, car, delta);

   this->position.x = car->position.x;
   this->position.z = car->position.y;
   this->rotation.y = car->rotation - M_PI / 2;

   float new_x = this->position.x - car->direction.x * 20;
   float new_z = this->position.z - car->direction.y * 20;

   vec2 angle_comp = { cos(car->rotation), sin(car->rotation) };
   vec2 angle_offset = { 2.75, 1.5 };
   my_wheels[0]->position.x = car->position.x + angle_offset.x * angle_comp.x + angle_offset.y * angle_comp.y;
   my_wheels[0]->position.z = car->position.y + angle_offset.x * angle_comp.y - angle_offset.y * angle_comp.x;
   my_wheels[0]->rotation.y = wheel_angle + car->rotation - M_PI / 2;

   my_wheels[1]->position.x = car->position.x + angle_offset.x * angle_comp.x - angle_offset.y * angle_comp.y * 2;
   my_wheels[1]->position.z = car->position.y + angle_offset.x * angle_comp.y + angle_offset.y * angle_comp.x * 2;
   my_wheels[1]->rotation.y = wheel_angle + car->rotation - M_PI / 2;

   // const float camera_k = 0.152f;
   // car_camera->position[0] = new_x * camera_k + car_camera->position[0] * (1 - camera_k);
   // car_camera->position[1] = 13;
   // car_camera->position[2] = new_z * camera_k + car_camera->position[2] * (1 - camera_k);

   // const float camera_dir_k = 0.15f;
   // car_camera->direction[0] = -car->direction.x * camera_dir_k + car_camera->direction[0] * (1 - camera_dir_k);
   // car_camera->direction[1] = 0.3f;
   // car_camera->direction[2] = -car->direction.y * camera_dir_k + car_camera->direction[2] * (1 - camera_dir_k);
   // vec4_norm(car_camera->direction);

   uint64_t frame = u_get_frames();
   if(frame % 2 == 0)
   {
      scene_t* scene = scm_get_current();
      const float diffX = this->position.x - car_start[0];
      const float diffY = this->position.y - car_start[1];
      const float diffZ = this->position.z - car_start[2];

      const float lt_off_x = -17.359648f;
      const float lt_off_y = 50.372597f;
      const float lt_off_z = 6;

      scene->shadow_light->light_camera->target[0] = diffX;
      //scene->shadow_light->light_camera->target[1] = 0;
      scene->shadow_light->light_camera->target[2] = diffZ;

      scene->shadow_light->position[0] = diffX + lt_off_x;
      scene->shadow_light->position[1] = lt_off_y;
      scene->shadow_light->position[2] = diffZ + lt_off_z;
      update_shadow_light();
   }
}

object_t* create_controllable_car(vec2 position, float rotation, camera_t* camera)
{
   object_t* this = o_create();
   this->draw_info->drawable = true;
   this->draw_info->model = rm_getn(MODEL, "car");
   this->draw_info->material = rm_getn(MATERIAL, "car1");

   this->position.y = 1.5;

   this->scale = vec3f(10, 10, 10);
   this->update_func = update_controllable_car;
   car_camera = camera;
   //car_start = cvec4(position.x, 0, position.y, 0);
   car_start = cvec4(0, 0, 0, 0);

   car = DEEPCARS_MALLOC(sizeof(car_t));
   car->position = position;
   car->rotation = rotation;
   car->velocity = vec2e;
   car->rot_velocity = 0;

   for(size_t i = 0; i < 4; i++)
   {
      my_wheels[i] = create_textured_dummy(
            vec3f(0, 0, 0), 2,
            rm_getn(MATERIAL, "wall"),
            rm_getn(MODEL, "wheels"));
      u_push_object(my_wheels[i]);
   }

   return this;
}


