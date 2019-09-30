#include "lib/graphics/win.h"
#include "lib/shaders/shaderMgr.h"
#include "lib/updater.h"
#include "lib/graphics/graphics.h"

int main(int argc, char* argv[])
{
    winInfo_t* win = w_create(1200, 800, 0, 0, "hello world", false, stdout);
    w_printInfo();
    s_init();
    gr_init();

    u_startLoop(win, NULL, NULL);

    s_free();
    gr_free();
    w_destroy(win);
}