//
// Created by maxim on 2/18/20.
//

#ifndef DEEPCARS_OBJ_EDITOR_DRAWER_H
#define DEEPCARS_OBJ_EDITOR_DRAWER_H

#include "../../../lib/object_include.h"

extern enum _toolbar_state {
   ERASER,
   OBSTACLE,
   SLIP,
   START,
   FIN,
   WALL,

} toolbar_state;

extern enum _tab_state {
   TAB_FFNN,
   TAB_GA,
   TAB_MAP,
   TAB_FILE,
   TAB_CLOSED

} tab_state;

extern uint8_t grid_state;
extern uint8_t current_grid_size;
extern void (*field_click_func)(uint32_t x, uint32_t y, uint32_t state, uint32_t mouse);

object_t* create_editor_drawer(void);

#endif //DEEPCARS_OBJ_EDITOR_DRAWER_H
