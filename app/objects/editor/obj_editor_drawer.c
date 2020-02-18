//
// Created by maxim on 2/18/20.
//

#ifdef __GNUC__
#pragma implementation "obj_editor_drawer.h"
#endif
#include "obj_editor_drawer.h"
#include "../../../lib/resources/rmanager.h"

texture_t* texture;

void update_editor(object_t* this)
{
   float a = 1;
   gr_pq_push_sprite(0, texture, vec2f(0, 0), vec2f(1, 1),
         vec2f(0, 0), 0, true, &a);

   gr_pq_push_sprite(0, texture, vec2f(5, 5), vec2f(1, 1),
                     vec2f(0, 0), 0, true, &a);
}

object_t* create_editor_drawer(void)
{
   texture = rm_getn(TEXTURE, "logo");

   object_t* this = o_create();
   this->update_func = update_editor;
   return this;
}


