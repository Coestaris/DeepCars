//
// Created by maxim on 2/20/20.
//

#ifndef DEEPCARS_OBJ_EDITOR_MAP_H
#define DEEPCARS_OBJ_EDITOR_MAP_H

#include "../../../lib/object_include.h"
#define GET_INTERSECTION_COMP 1e-5

extern list_t* walls;
extern list_t* map_objects;
extern vec2 prev_point;
extern bool first_point_set;

object_t* create_editor_map(void);

// taken from https://github.com/Coestaris/Zomboid2.0/blob/master/client/lib/ltracer/ltracer_math.c
bool get_intersection(double ray_x1, double ray_y1, double ray_x2, double ray_y2, vec2 seg1, vec2  seg2,
                      double* rx, double* ry, double* dist);

#endif //DEEPCARS_OBJ_EDITOR_MAP_H
