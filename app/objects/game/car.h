//
// Created by maxim on 3/19/20.
//

#ifndef DEEPCARS_CAR_H
#define DEEPCARS_CAR_H

#include "../../win_defaults.h"

#define CAR_INPUT_L 0
#define CAR_INPUT_R 1
#define CAR_INPUT_F 2
#define CAR_INPUT_B 3

typedef struct {
   vec2 position;
   float rotation;

   vec2 velocity;
   float rot_velocity;

   vec2 direction;

} car_t;

void car_update(bool inputs[4], car_t* car, float delta);

#endif //DEEPCARS_CAR_H
