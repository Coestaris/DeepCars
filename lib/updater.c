//
// Created by maxim on 8/28/19.
//

#include "updater.h"
#include "graphics/graphics.h"

#define OBJECT_COUNT_START 50
#define OBJECT_COUNT_INCREASE 1.5

bool closed = false;
uint64_t counter = 0;
double elapsed = 0;
double fps = 0;
uint64_t frames;

uint32_t keysState[256];
uint32_t mouseState[10];
uint32_t mouseX;
uint32_t mouseY;
winInfo_t* defaultWin;

struct list {
    size_t count;
    size_t maxSize;

    drawableObject_t** collection;
};

struct list* objects;
struct list* keyListeners;
struct list* mouseListeners;
struct list* mouseMoveListeners;
struct list* updateListeners;

void listPush(struct list* list, drawableObject_t* drawableObject)
{
    list->collection[list->count++] = drawableObject;
}

void listRemove(struct list* list, drawableObject_t* drawableObject)
{
    //todo
}


double getMillis(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0;
}

void u_close(void)
{
    closed = true;
}

void drawFunc()
{
    gr_fill(COLOR_GRAY);
    w_swapBuffers(defaultWin);
}

void eventHandler(XEvent event)
{
    switch(event.type)
    {
        case KeyPress:
        {
            keysState[event.xkey.keycode] = 1;
            for(size_t i = 0; i < keyListeners->count; i++)
                keyListeners->collection[i]->keyEventFunc(
                        keyListeners->collection[i],
                        event.xkey.keycode,
                        event.xkey.state);
        }
            break;
        case KeyRelease:
            keysState[event.xkey.keycode] = 0;
            for(size_t i = 0; i < keyListeners->count; i++)
                keyListeners->collection[i]->keyEventFunc(
                        keyListeners->collection[i],
                        event.xkey.keycode,
                        event.xkey.state);
            break;
        case ButtonPress:
            mouseState[event.xbutton.button] = 1;
            for(size_t i = 0; i < mouseListeners->count; i++)
                mouseListeners->collection[i]->mouseEventFunc(
                        mouseListeners->collection[i],
                        event.xbutton.x,
                        event.xbutton.y,
                        event.xbutton.state,
                        event.xbutton.button);
            break;
        case ButtonRelease:
            mouseState[event.xbutton.button] = 0;
            for(size_t i = 0; i < mouseListeners->count; i++)
                mouseListeners->collection[i]->mouseEventFunc(
                        mouseListeners->collection[i],
                        event.xbutton.x,
                        event.xbutton.y,
                        event.xbutton.state,
                        event.xbutton.button);
            break;
        case MotionNotify:
            mouseX = event.xbutton.x;
            mouseY = event.xbutton.y;
            for(size_t i = 0; i < mouseMoveListeners->count; i++)
                mouseMoveListeners->collection[i]->mouseMoveEventFunc(
                        mouseMoveListeners->collection[i],
                        event.xbutton.x,
                        event.xbutton.y);
            break;
        case Expose:
            break;
    }
}

void u_pushObject(drawableObject_t* object)
{
    listPush(objects, object);
    if(object->mouseEventFunc)
        listPush(mouseListeners, object);
    if(object->keyEventFunc)
        listPush(keyListeners, object);
    if(object->keyEventFunc)
        listPush(keyListeners, object);
    if(object->mouseMoveEventFunc)
        listPush(mouseMoveListeners, object);
}

uint64_t u_getFrames()
{
    return frames;
}

void measureTime(void)
{
    double tickStart = getMillis();

    drawFunc();

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
        fps = 1000.0 * counter / elapsed;
        counter = 0;
        elapsed = 0;

        printf("[updater.c]: FPS: %lf\n", fps);
    }
}

void u_startLoop(winInfo_t* win)
{
    objects = malloc(sizeof(struct list));
    objects->count = 0;
    objects->maxSize = OBJECT_COUNT_START;
    keyListeners = malloc(sizeof(struct list));
    keyListeners->count = 0;
    keyListeners->maxSize = OBJECT_COUNT_START;
    mouseListeners = malloc(sizeof(struct list));
    mouseListeners->count = 0;
    mouseListeners->maxSize = OBJECT_COUNT_START;
    mouseMoveListeners = malloc(sizeof(struct list));
    mouseMoveListeners->count = 0;
    mouseMoveListeners->maxSize = OBJECT_COUNT_START;
    updateListeners = malloc(sizeof(struct list));
    updateListeners->count = 0;
    updateListeners->maxSize = OBJECT_COUNT_START;
    XEvent event;

    XSelectInput(win->display, win->win,
                 KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | ButtonMotionMask | ExposureMask | VisibilityChangeMask |
                 ResizeRedirectMask);
    while (1)
    {
        if(closed) break;

        if(XPending(win->display) == 0)
        {
            for(size_t i = 0; i < updateListeners->count; i++)
                updateListeners->collection[i]->updateFunc(updateListeners->collection[i]);

            measureTime();
            continue;
        }

        XNextEvent(win->display, &event);
        eventHandler(event);
    }
}

