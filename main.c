#include "graphics/win.h"

void w_printInfo()
{
    printf("[GL Spec]: Vendor: %s\n", (char*) glGetString(GL_VENDOR));
    printf("[GL Spec]: Using OpenGL Version: %s\n", (char*) glGetString(GL_VERSION));
    printf("[GL Spec]: Using OpenGL Rendered: %s\n", (char*) glGetString(GL_RENDERER));
    printf("[GL Spec]: GLSH Version: %s\n=========\n", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void drawingRoutine(winInfo_t* win)
{
    glClearColor(0, 0.5, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    w_swapBuffers(win);

    sleep(1);

    glClearColor(1, 0.5, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    w_swapBuffers(win);

    sleep(1);

    glClearColor(0, 0.5, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    w_swapBuffers(win);

    sleep(1);
}

int main(int argc, char* argv[])
{
    winInfo_t* win = w_create(200, 200, 0, 0, "hello world", true, stdout);

    w_printInfo();

    drawingRoutine(win);

    w_destroy(win);
}