#include <sys/time.h>
#include "graphics/win.h"

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
            printf( "KeyPress: %x\n", event.xkey.keycode );
            break;
        case KeyRelease:
            printf( "KeyRelease: %x\n", event.xkey.keycode );
            break;
        case ButtonPress:
            printf("buttonPress: %x, x: %i, y: %i\n",
                   event.xbutton.button, event.xbutton.x, event.xbutton.y);
        case ButtonRelease:
            printf("buttonRelease: %x, x: %i, y: %i\n",
                   event.xbutton.button, event.xbutton.x, event.xbutton.y);
            break;

        case Expose:

            break;
    }
}

double getMillis(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000.0;
}

void drawingRoutine()
{
    glClearColor(drand48(), drand48(), drand48(), 1);
    glClear(GL_COLOR_BUFFER_BIT);
    w_swapBuffers(win);
}

int counter = 0;
double elapsed = 0;
double fps = 0;
int mousex = -1, mousey = -1;
int fixedW = -1, fixedH = -1;
long long frames;

#define FPSToLock 60.0
#define FPSDelay 1000.0 / FPSToLock
#define FPSAvCounter 1000.0

void measureTime()
{
    double tickStart = getMillis();

    drawingRoutine();

    double diff = getMillis() - tickStart;
    counter++;
    frames++;

    if (diff < FPSDelay)
    {
        usleep((unsigned int) (FPSDelay - diff) * 1000);
    }

    elapsed += getMillis() - tickStart;


    if (elapsed > FPSAvCounter)
    {
        fps = 1000 * counter / elapsed;
        counter = 0;
        elapsed = 0;

        printf("FPS: %lf\n", fps);
    }
}

void eventRoutine()
{
    XEvent event;

    XSelectInput(win->display, win->win,
            KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
            PointerMotionMask | ButtonMotionMask | ExposureMask | VisibilityChangeMask |
            ResizeRedirectMask);

    bool closed = 0;

    while (1)
    {
        if(closed) break;

        if(XPending(win->display) == 0)
        {
            measureTime();
            continue;
        }

        XNextEvent(win->display, &event);
        proceedEvent(event);
    }
}

int main(int argc, char* argv[])
{
    win = w_create(500, 600, 0, 0, "hello world", false, stdout);

    w_printInfo();

    eventRoutine();

    w_destroy(win);
}