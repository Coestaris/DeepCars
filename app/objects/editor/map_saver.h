//
// Created by maxim on 2/24/20.
//

#ifndef DEEPCARS_MAP_SAVER_H
#define DEEPCARS_MAP_SAVER_H

#include "../../../lib/structs.h"
#include "obj_editor_drawer.h"

#define MAP_SAVE_DIR "maps"

typedef struct _wall {
   vec2 p1;
   vec2 p2;
} wall_t;

typedef struct _map_object {
   vec2 pos;

   vec2 p1;
   vec2 p2;

   float rotation;
   enum _toolbar_state type;
} map_object_t;

void map_save(list_t* walls, list_t* objects, char* file, vec2 prev_point, bool first_point_set);
void map_load(list_t* walls, list_t* objects, char* file, vec2* prev_point, bool* first_point_set);

#endif //DEEPCARS_MAP_SAVER_H
