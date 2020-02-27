//
// Created by maxim on 2/20/20.
//

#ifndef DEEPCARS_OBJ_EDITOR_MAP_H
#define DEEPCARS_OBJ_EDITOR_MAP_H

#include "../../../lib/object_include.h"

extern list_t* walls;
extern list_t* map_objects;
extern vec2f_t prev_point;
extern bool first_point_set;

object_t* create_editor_map(void);

#endif //DEEPCARS_OBJ_EDITOR_MAP_H
