//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_CHAIN_H
#define DEEPCARS_RENDER_CHAIN_H

#include "../../coredefs.h"

#include "render_stage.h"
#include "../../coredefs.h"
#include "../../object.h"
#include "../../resources/shader.h"
#include "../../resources/shm.h"
#include "../win.h"

// Represent list of rendering stages
typedef struct _render_chain
{
   list_t* stages;

   // Chain user data
   void* data;

} render_chain_t;

// render_chain_t constructor
render_chain_t* rc_create();

// Frees render_chain_t but doesn't free its stages
void rc_free(render_chain_t* rc, bool free_stages);

// Link stages between each other (like linked list)
void rc_link(render_chain_t* rc);

// Calls rs_build_tex depends on its mode
void rc_build(render_chain_t* rc);

// Updates current rendering chain and linking it
void rc_set_current(render_chain_t* rc);

// Returns current rendering chain
render_chain_t* rc_get_current(void);

#endif //DEEPCARS_RENDER_CHAIN_H
