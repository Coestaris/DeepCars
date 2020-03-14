//
// Created by maxim on 3/14/20.
//

#ifdef __GNUC__
#pragma implementation "obj_controllable_car.h"
#endif
#include "obj_controllable_car.h"
#include "../../rendering/renderer.h"

static camera_t* car_camera;
static vec2 car_velocity = vec2e;
static float rot_velocity = 0;
static const float car_acc = .05f;
static const float max_car_velocity = 2.3f;
static const float rot_acc = .002f;
static const float max_rot_velocity = .02f;
static vec4 car_start = NULL;

static void update_controllable_car(object_t* this)
{
   vec2 direction = vec2f(cos(this->rotation.y), sin(this->rotation.y));
   vec2 normal = vec2_normal(direction);

   if(u_get_key_state(111) == KEY_PRESSED) // up
   {
      if(vec2_len(car_velocity) < max_car_velocity * 2)
      {
         car_velocity.x += direction.x * car_acc;
         car_velocity.y += direction.y * car_acc;
      }
   }

   float vel_to_dir1 = vec2_dot(car_velocity, direction) / vec2_len(direction);
   float vel_to_norm1 = vec2_dot(car_velocity, normal) / vec2_len(normal);
   car_velocity.x = 0.9 * vel_to_norm1 * normal.x + 0.95 * vel_to_dir1 * direction.x;
   car_velocity.y = 0.9 * vel_to_norm1 * normal.y + 0.95 * vel_to_dir1 * direction.y;

   if(u_get_key_state(116) == KEY_PRESSED) // down
   {
      //vec2 direction = vec2f(cos(this->rotation.y), sin(this->rotation.y));
      car_velocity.x -= direction.x * car_acc / 2;
      car_velocity.y -= direction.y * car_acc / 2;
   }

   if(u_get_key_state(113) == KEY_PRESSED) // r
   {
      if(u_get_key_state(116) == KEY_PRESSED)
      {
         if(rot_velocity < max_rot_velocity)
            rot_velocity += rot_acc;
      }
      else
      {
         if(rot_velocity > -max_rot_velocity)
            rot_velocity -= rot_acc;
      }
   }
   else if(u_get_key_state(114) == KEY_PRESSED) // l
   {
      if(u_get_key_state(116) == KEY_PRESSED)
      {
         if(rot_velocity > -max_rot_velocity)
            rot_velocity -= rot_acc;
      }
      else
      {
         if(rot_velocity < max_rot_velocity)
            rot_velocity += rot_acc;
      }
   }
   else rot_velocity *= 0.9f;

   float vel_to_dir = vec2_dot(car_velocity, direction) / vec2_len(direction);
   float vel_to_norm = vec2_dot(car_velocity, normal) / vec2_len(normal);
   vel_to_norm *= 0.6;

   vec2 scaled_velocity = vec2f(
         vel_to_dir * direction.x + vel_to_norm * normal.x,
         vel_to_dir * direction.y + vel_to_norm * normal.y);
   float vel = vec2_len(scaled_velocity);
   float ratio = vel / max_car_velocity;
   if(ratio > 1)
   {
      scaled_velocity.x *= max_car_velocity / vel;
      scaled_velocity.y *= max_car_velocity / vel;
   }

   this->position.x += scaled_velocity.x;
   this->position.z += scaled_velocity.y;

   this->rotation.y += rot_velocity;

   float new_x = this->position.x - direction.x * 22;
   float new_z = this->position.z - direction.y * 22;
   car_camera->position[0] = new_x * 0.5f + car_camera->position[0] * (1 - 0.5f);
   car_camera->position[1] = 13;
   car_camera->position[2] = new_z * 0.5f + car_camera->position[2] * (1 - 0.5f);

   car_camera->direction[0] = -direction.x * 0.5f + car_camera->direction[0] * (1 - 0.5f);
   car_camera->direction[1] = 0.3f;
   car_camera->direction[2] = -direction.y * 0.5f + car_camera->direction[2] * (1 - 0.5f);
   vec4_norm(car_camera->direction);

   uint64_t frame = u_get_frames();
   if(frame % 15 == 0)
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
   this->position = vec3f(position.x, 0, position.y);
   this->rotation = vec3f(0, rotation, 0);

   this->draw_info->drawable = true;
   this->draw_info->model = rm_getn(MODEL, "car");
   this->draw_info->material = rm_getn(MATERIAL, "car1");

   this->scale = vec3f(5, 5, 5);
   this->update_func = update_controllable_car;
   car_camera = camera;
   car_start = cvec4(position.x, 0, position.y, 0);
   return this;
}


