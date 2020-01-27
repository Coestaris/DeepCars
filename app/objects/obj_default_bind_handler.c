//
// Created by maxim on 1/26/20.
//

#ifdef __GNUC__
#pragma implementation "obj_default_bind_handler.h"
#endif
#include "obj_default_bind_handler.h"
#include "../rendering/renderer.h"

void camera_key_event_func(object_t* this, uint32_t key, uint32_t state)
{
   if (key == 9) //esc
      u_close();
   if (key == 65 && state == KEY_RELEASE) //shift
      switch_ssao();
   if (key == 106 && state == KEY_RELEASE) //cntr
      switch_stages();
}

object_t* create_default_bind_handler()
{
   object_t* this = o_create();
   this->key_event_func = camera_key_event_func;
   return this;
}
