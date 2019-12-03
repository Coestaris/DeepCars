//
// Created by maxim on 11/30/19.
//

#ifdef __GNUC__
#pragma implementation "render_chain.h"
#endif
#include "render_chain.h"
#include "../win.h"

render_chain_t* rc_create()
{
   render_chain_t* this = malloc(sizeof(render_chain_t));
   this->data = NULL;
   this->stages = list_create(10);
   return this;
}

win_info_t* default_win;

void create_perspective(mat4 mat, float fov, float near, float far)
{
   mat4_perspective_fov(mat, fov, (float)default_win->w / (float)default_win->h, near, far);
}

void create_ortho(mat4 mat, float near, float far)
{
   mat4_ortho(mat, near, far, default_win->w, default_win->h);
}

render_chain_t* rc_default()
{
   shader_t* default_shader = s_get_shader(SH_COLORED);
   shader_t* gaussian_shader = s_get_shader(SH_BLUR);
   shader_t* bypass_shader = s_get_shader(SH_BYPASS);

   render_stage_t* geometry = rs_create(true, default_shader);
   geometry->tex_width = default_win->w;
   geometry->tex_height = default_win->h;
   create_perspective(geometry->proj, 60.0f, 0.1f, 200);
   mat4_identity(geometry->view);

   render_stage_t* blur = rs_create(true, gaussian_shader);
   geometry->tex_width = default_win->w;
   geometry->tex_height = default_win->h;
   create_ortho(geometry->proj, 0.1f, 200);
   mat4_identity(geometry->view);

   render_stage_t* bypass = rs_create(true, bypass_shader);
   bypass->tex_width = default_win->w;
   bypass->tex_height = default_win->h;
   create_ortho(bypass->proj, 0.1f, 200);
   mat4_identity(bypass->view);
   bypass->final = true;

   render_chain_t* rc = rc_create();
   list_push(rc->stages, geometry);
   list_push(rc->stages, blur);
   list_push(rc->stages, bypass);
   return rc;
}


