//
// Created by maxim on 12/30/19.
//

#ifndef DEEPCARS_RENDERER_H
#define DEEPCARS_RENDERER_H

#include "../../lib/coredefs.h"

#include "../../lib/graphics/rendering/render_chain.h"
#include "../../lib/object_include.h"

#define STAGE_G_BUFFER 0
#define STAGE_G_BUFFER_INST 1
#define STAGE_NORMAL 2
#define STAGE_SSAO 3
#define STAGE_SSAO_BLUR 4
#define STAGE_SKYBOX 5
#define STAGE_SHADOWMAP 6
#define STAGE_SHADOWMAP_INST 7
#define STAGE_SHADING 8
#define STAGE_BYPASS 9
#define STAGE_FXAA 10
#define STAGE_PRIMITIVE 11

extern uint32_t   render_state;

extern texture_t* texture_to_draw;
extern uint32_t   fxaa_state;
extern uint32_t   fxaa_edges;

extern shader_t*  br_shader;
extern uint32_t   wireframe;
extern texture_t* noise_texture;
extern texture_t* ssao_texture;
extern sprite_renderer_t* default_sprite_renderer;
extern primitive_renderer_t* default_primitive_renderer;

extern render_chain_t* default_rc;
extern render_chain_t* editor_rc;
extern list_t* blurred_regions;

char* get_stage_string();

void switch_stages(void);
void switch_ssao(void);
void switch_wireframe(void);
void switch_fxaa(void);
void switch_fxaa_edges(void);
void free_stages(void);
render_chain_t* get_chain(win_info_t* win, camera_t* camera, mat4 proj);

void update_shadow_light(void);
void update_lights(void);

#endif //DEEPCARS_RENDERER_H
