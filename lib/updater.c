//
// Created by maxim on 8/28/19.
//

#include "updater.h"
#include "graphics/graphics.h"

bool closed = false;
int counter = 0;
double elapsed = 0;
double fps = 0;
long long frames;
drawFunc_t globalDrawFunc;

double getMillis(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000.0;
}

void u_close(void)
{
    closed = true;
}

void measureTime(void)
{
    double tickStart = getMillis();

    globalDrawFunc();

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

int keysState[256];
int mouseState[10];
int mousePressX[10];
int mousePressY[10];
winInfo_t* defaultWin;

void defaultDrawFunc()
{
    gr_fill(COLOR_GRAY);
    w_swapBuffers(defaultWin);
}

void defaultEventHandler()
{

}

void u_startLoop(winInfo_t* win, drawFunc_t drawFunc, eventHandler_t eventHandler)
{
    XEvent event;

    XSelectInput(win->display, win->win,
                 KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | ButtonMotionMask | ExposureMask | VisibilityChangeMask |
                 ResizeRedirectMask);

    if(drawFunc == NULL)
    {
        globalDrawFunc = defaultDrawFunc;
        defaultWin = win;
    }
    else
        globalDrawFunc = drawFunc;

    if(eventHandler == NULL)
        eventHandler = defaultEventHandler;

    while (1)
    {
        if(closed) break;

        if(XPending(win->display) == 0)
        {
            measureTime();
            continue;
        }

        XNextEvent(win->display, &event);
        eventHandler(event);
    }
}

