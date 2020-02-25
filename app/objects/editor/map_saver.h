//
// Created by maxim on 2/24/20.
//

#ifndef DEEPCARS_MAP_SAVER_H
#define DEEPCARS_MAP_SAVER_H

#include "../../../lib/structs.h"
#include "obj_editor_drawer.h"

typedef struct _wall {
   vec2f_t p1;
   vec2f_t p2;
} wall_t;

typedef struct _map_object {
   vec2f_t pos;

   vec2f_t p1;
   vec2f_t p2;

   float rotation;
   enum _toolbar_state type;
} map_object_t;

void map_save(list_t* walls, list_t* objects, char* file);
void map_load(list_t* walls, list_t* objects, char* file);

#endif //DEEPCARS_MAP_SAVER_H