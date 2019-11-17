//
// Created by maxim on 11/17/19.
//

#include "loader.h"
#include "../lib/graphics/win.h"
#include "../lib/graphics/graphics.h"
#include "../lib/updater.h"
#include "../lib/shaders/shaderMgr.h"

#include "winDefaults.h"

winInfo_t* win;
mat4 view;
mat4 model;

void app_loadResources(void)
{

}

void app_initGraphics(void)
{
    win = w_create(
            WIN_WIDTH,
            WIN_HEIGHT,
            0, 0,
            WIN_TITLE,
            VERBOSE,
            stdout);

    const float angleOfView = 60.0f;
    const float near = 0.1f;
    const float far = 100.0f;
    const float imageAspectRatio = (float)win->w / (float)win->h;
    win->projection = cmat4();
    perspectiveFovMat(win->projection, angleOfView, imageAspectRatio, near, far);

    view = cmat4();
    model = cmat4();

    if(VERBOSE)
        w_printInfo();

    s_init();
    gr_init(win->projection, view);
}

void app_run(void)
{
    u_startLoop(win, NULL, NULL);
}

void finalize()
{
    s_free();
    w_destroy(win);
}