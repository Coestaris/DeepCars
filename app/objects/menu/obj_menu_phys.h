//
// Created by maxim on 1/28/20.
//

#ifndef DEEPCARS_OBJ_MENU_PHYS_H
#define DEEPCARS_OBJ_MENU_PHYS_H

#include "../../../lib/object_include.h"

#define SPHERES_COUNT 30
#define SPHERE_RADIUS 5
#define CAR_SIZE 15

extern object_t* render_spheres[SPHERES_COUNT];
extern object_t* render_car1;
extern object_t* render_car2;

object_t* create_menu_phys(void);

#endif //DEEPCARS_OBJ_MENU_PHYS_H
