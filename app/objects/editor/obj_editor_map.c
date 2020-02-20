//
// Created by maxim on 2/20/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_map.h"
#endif
#include "obj_editor_map.h"
#include "../../rendering/renderer.h"

void update_editor_map(object_t* this)
{
   vec2f_t pos = u_get_mouse_pos();
   gr_pq_push_line(2, vec2f(10, 10), pos, 4, COLOR_WHITE, default_primitive_renderer, NULL);
}

object_t* create_editor_map(void)
{
   object_t* this = o_create();
   this->update_func = update_editor_map;
   return this;
}
