//
// Created by maxim on 8/26/19.
//

#ifndef ZOMBOID3D_GRAPHICS_H
#define ZOMBOID3D_GRAPHICS_H

#include <GL/gl.h>
#include "gmath.h"

void gr_fill(vec4 color);

void gr_init(void);
void gr_free(void);

extern vec4 COLOR_WHITE;
extern vec4 COLOR_SILVER;
extern vec4 COLOR_GRAY;
extern vec4 COLOR_BLACK;
extern vec4 COLOR_RED;
extern vec4 COLOR_MAROON;
extern vec4 COLOR_YELLOW;
extern vec4 COLOR_OLIVE;
extern vec4 COLOR_LIME;
extern vec4 COLOR_GREEN;
extern vec4 COLOR_AQUA;
extern vec4 COLOR_TEAL;
extern vec4 COLOR_BLUE;
extern vec4 COLOR_NAVY;
extern vec4 COLOR_FUCHSIA;
extern vec4 COLOR_PURPLE;

#endif //ZOMBOID3D_GRAPHICS_H
