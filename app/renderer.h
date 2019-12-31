//
// Created by maxim on 12/30/19.
//

#ifndef DEEPCARS_RENDERER_H
#define DEEPCARS_RENDERER_H

#include "../lib/graphics/rendering/render_chain.h"

#define SH_DEFAULT 0
#define SH_BLUR 1
#define SH_BYPASS 2
#define SH_SKYBOX 3
#define SH_DEPTH_SHADER 4
#define SH_DEPTH_BYPASS 5

void switch_stages(void);
render_chain_t* get_chain(win_info_t* win, camera_t* camera, mat4 proj);

#endif //DEEPCARS_RENDERER_H
