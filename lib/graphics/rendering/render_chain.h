//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_CHAIN_H
#define DEEPCARS_RENDER_CHAIN_H

#include <stdbool.h>

#include "render_stage.h"
#include "../../structs.h"
#include "../../object.h"
#include "../../shaders/shader.h"
#include "../../shaders/shm.h"
#include "../win.h"

typedef struct _render_chain {

   list_t* stages;
   void* data;

} render_chain_t;

render_chain_t* rc_create();
void rc_set_current(render_chain_t* rc);
render_chain_t* rc_get_current(void);

render_chain_t* rc_default(win_info_t* info);

#endif //DEEPCARS_RENDER_CHAIN_H
