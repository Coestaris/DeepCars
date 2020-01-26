//
// Created by maxim on 12/30/19.
//

#ifndef DEEPCARS_RENDERER_H
#define DEEPCARS_RENDERER_H

#include "../../lib/graphics/rendering/render_chain.h"

#define STAGE_G_BUFFER 0
#define STAGE_SSAO 1
#define STAGE_SSAO_BLUR 2
#define STAGE_SKYBOX 3
#define STAGE_SHADOWMAP 4
#define STAGE_SHADING 5
#define STAGE_BYPASS 6
#define STAGE_PRIMITIVE 7

extern render_chain_t* rc;
extern list_t* blurred_regions;

char* get_ssao_stage_string();

void switch_stages(void);
void switch_ssao(void);
void free_stages(void);
render_chain_t* get_chain(win_info_t* win, camera_t* camera, mat4 proj);

void update_shadow_light(void);
void update_lights(void);

#endif //DEEPCARS_RENDERER_H
