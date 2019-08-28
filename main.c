#include "graphics/win.h"
#include "updater.h"

void w_printInfo()
{
    printf("[GL Spec]: Vendor: %s\n", (char*) glGetString(GL_VENDOR));
    printf("[GL Spec]: Using OpenGL Version: %s\n", (char*) glGetString(GL_VERSION));
    printf("[GL Spec]: Using OpenGL Rendered: %s\n", (char*) glGetString(GL_RENDERER));
    printf("[GL Spec]: GLSH Version: %s\n", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}

winInfo_t* win;

void proceedEvent(XEvent event)
{
    switch(event.type)
    {
        case KeyPress:
            break;
        case KeyRelease:
            break;
        case ButtonPress:
            break;
        case ButtonRelease:
            break;
        case Expose:
            break;
    }
}


void drawingRoutine()
{
}

int main(int argc, char* argv[])
{
    win = w_create(500, 600, 0, 0, "hello world", false, stdout);
    w_printInfo();
    u_startLoop(win, drawingRoutine, proceedEvent);
    w_destroy(win);
}