//
// Created by maxim on 1/26/20.
//

#ifndef DEEPCARS_OBJ_MENU_CAMERA_MOVER_H
#define DEEPCARS_OBJ_MENU_CAMERA_MOVER_H

#include "../../../lib/object_include.h"

static const float default_camera_y = 49.372597f;
static const float default_camera_r = 102;

void update_camera(float new_y, float new_r);

object_t* create_menu_camera_mover();

#endif //DEEPCARS_OBJ_MENU_CAMERA_MOVER_H
