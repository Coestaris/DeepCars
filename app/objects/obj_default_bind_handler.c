//
// Created by maxim on 1/26/20.
//

#ifdef __GNUC__
#pragma implementation "obj_default_bind_handler.h"
#endif
#include "obj_default_bind_handler.h"
#include "../rendering/renderer.h"
#include "obj_info_draw.h"

void camera_key_event_func(object_t* this, uint32_t key, uint32_t state)
{
   if (key == 9) //esc
      u_close();
   else if (key == 62 && state == KEY_RELEASE) //shift
      switch_ssao();
   else if (key == 105 && state == KEY_RELEASE) //cntr
      switch_stages();
   else if (key == 108 && state == KEY_RELEASE) //alt
      switch_fxaa();
   else if (key == 59 && state == KEY_RELEASE) //alt
      switch_fxaa_edges();
   else if (key == 135 && state == KEY_RELEASE) //popup menu
      switch_text_drawer();
   else if (key == 61 && state == KEY_RELEASE) // dot
      switch_wireframe();
}

object_t* create_default_bind_handler()
{
   object_t* this = o_create();
   this->key_event_func = camera_key_event_func;
   return this;
}
