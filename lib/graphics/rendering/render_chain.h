//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_CHAIN_H
#define DEEPCARS_RENDER_CHAIN_H

#include <stdbool.h>

#include "render_stage.h"
#include "../../structs.h"
#include "../../object.h"
#include "../../resources/shader.h"
#include "../../resources/shm.h"
#include "../win.h"

typedef struct _render_chain {

   list_t* stages;
   void* data;

} render_chain_t;

void rc_create_perspective(win_info_t* win, mat4 mat, float fov, float near, float far);
void rc_create_ortho(win_info_t* win, mat4 mat, float near, float far);
GLint rc_get_quad_vao(void);
GLint rc_get_cube_vao(void);

void rc_link(render_chain_t* rc);
void rc_build(render_chain_t* rc);

render_chain_t* rc_create();
void rc_free(render_chain_t* rc, bool free_stages);

void rc_set_current(render_chain_t* rc);
render_chain_t* rc_get_current(void);

#endif //DEEPCARS_RENDER_CHAIN_H
