//
// Created by maxim on 12/30/19.
//

#ifndef DEEPCARS_RENDERER_H
#define DEEPCARS_RENDERER_H

#include "../../lib/graphics/rendering/render_chain.h"

void switch_stages(void);
void switch_ssao(void);
void free_stages(void);
render_chain_t* get_chain(win_info_t* win, camera_t* camera, mat4 proj);

void update_shadow_light(void);
void update_lights(void);

#endif //DEEPCARS_RENDERER_H
