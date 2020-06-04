//
// Created by maxim on 3/19/20.
//

#ifdef __GNUC__
#pragma implementation "car.h"
#endif
#include "car.h"

#define POSITION_ACCELERATION         0.05f
#define ROTATION_ACCELERATION         0.006f
#define MAX_VELOCITY                  2.3f
#define MAX_ROTATION_VELOCITY         0.035f
#define REVERSE_VELOCITY_MULTIPLIER   0.333f
#define REVERSE_ROTATION_MULTIPLIER   0.333f
#define NORMAL_FRICTION_COMPONENT     0.92f
#define DIRECTION_FRICTION_COMPONENT  0.95f
#define NORMAL_VELOCITY_DECREASE      0.7f
#define ROTATION_VELOCITY_DECREASE    0.9f
#define ROTATION_DECREASE_MAX         30
#define ROTATION_DECREASE_MULTIPLIER  0.5f

void car_update(bool inputs[4], car_t* car, float delta)
{
   car->direction = vec2f(cosf(car->rotation), sinf(car->rotation));
   vec2 normal    = vec2_normal(car->direction);

   if(inputs[CAR_INPUT_F]) // up
   {
      if(vec2_len(car->velocity) < MAX_VELOCITY * 2)
      {
         car->velocity.x += car->direction.x * POSITION_ACCELERATION;
         car->velocity.y += car->direction.y * POSITION_ACCELERATION;
      }
   }

   float vel_to_dir1  = (float)vec2_dot(car->velocity, car->direction) / (float)vec2_len(car->direction);
   float vel_to_norm1 = (float)vec2_dot(car->velocity, normal) / (float)vec2_len(normal);
   car->velocity.x =
         NORMAL_FRICTION_COMPONENT * vel_to_norm1 * normal.x +
         DIRECTION_FRICTION_COMPONENT * vel_to_dir1 * car->direction.x;

   car->velocity.y =
         NORMAL_FRICTION_COMPONENT * vel_to_norm1 * normal.y +
         DIRECTION_FRICTION_COMPONENT * vel_to_dir1 * car->direction.y;

   if(inputs[CAR_INPUT_B]) // down
   {
      car->velocity.x -= car->direction.x * POSITION_ACCELERATION * REVERSE_VELOCITY_MULTIPLIER;
      car->velocity.y -= car->direction.y * POSITION_ACCELERATION * REVERSE_VELOCITY_MULTIPLIER;
   }


   float vel_to_dir  = (float)vec2_dot(car->velocity, car->direction) / (float)vec2_len(car->direction);
   float vel_to_norm = (float)vec2_dot(car->velocity, normal) / (float)vec2_len(normal);
   vel_to_norm *= NORMAL_VELOCITY_DECREASE;

   vec2 scaled_velocity = vec2f(
         vel_to_dir * car->direction.x + vel_to_norm * normal.x,
         vel_to_dir * car->direction.y + vel_to_norm * normal.y);

   float vel   = (float)vec2_len(scaled_velocity);
   float ratio = vel / MAX_VELOCITY;

   if(ratio > 1)
   {
      scaled_velocity.x *= MAX_VELOCITY / vel;
      scaled_velocity.y *= MAX_VELOCITY / vel;
   }

   if(inputs[CAR_INPUT_R])
   {
      float a = fminf(MAX_VELOCITY / (float)vec2_len(scaled_velocity) *
                      ROTATION_DECREASE_MULTIPLIER, ROTATION_DECREASE_MAX);

      if(inputs[CAR_INPUT_B])
      {
         if(car->rot_velocity < MAX_ROTATION_VELOCITY)
            car->rot_velocity += ROTATION_ACCELERATION / a / REVERSE_ROTATION_MULTIPLIER;
      }
      else
      {
         if(car->rot_velocity > -MAX_ROTATION_VELOCITY)
            car->rot_velocity -= ROTATION_ACCELERATION / a;
      }
   }
   else if(inputs[CAR_INPUT_L]) // l
   {
      float a = fminf(MAX_VELOCITY / (float)vec2_len(scaled_velocity) *
                      ROTATION_DECREASE_MULTIPLIER, ROTATION_DECREASE_MAX);

      if(inputs[CAR_INPUT_B])
      {
         if(car->rot_velocity > -MAX_ROTATION_VELOCITY)
            car->rot_velocity -= ROTATION_ACCELERATION / a / REVERSE_ROTATION_MULTIPLIER;
      }
      else
      {
         if(car->rot_velocity < MAX_ROTATION_VELOCITY)
            car->rot_velocity += ROTATION_ACCELERATION / a;
      }
   }

   car->rot_velocity *= ROTATION_VELOCITY_DECREASE;
   car->position.x += scaled_velocity.x;
   car->position.y += scaled_velocity.y;

   car->rotation += car->rot_velocity;
   car->reverse = vec2_dot(car->direction, car->velocity) < 0;
}


