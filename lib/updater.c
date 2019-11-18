//
// Created by maxim on 8/28/19.
//

#include "updater.h"
#include "graphics/graphics.h"
#include "shaders/shader.h"

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

list_t* objects;
list_t* keyListeners;
list_t* mouseListeners;
list_t* mouseMoveListeners;
list_t* updateListeners;

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

    for(size_t i = 0; i < objects->count; i++)
    {
        drawableObject_t* object = (drawableObject_t*)objects->collection[i];
        if(object->model)
            gr_draw_model_simpleColor(object->model, object->color,
                    object->position, object->scale, object->rotation);
    }

    w_swapBuffers(defaultWin);
}

vec2f_t u_getMousePos()
{
    return vec2f(mouseX, mouseY);
}

int u_getKeyState(int key)
{
    return keysState[key];
}

int u_getMouseState(int mouse)
{
    return mouseState[mouse];
}

void eventHandler(XEvent event)
{
    switch(event.type)
    {
        case KeyPress:
        {
            keysState[event.xkey.keycode] = 1;
            for(size_t i = 0; i < keyListeners->count; i++)
                ((drawableObject_t*)keyListeners->collection[i])->keyEventFunc(
                        keyListeners->collection[i],
                        event.xkey.keycode,
                        KEY_PRESSED);
        }
            break;
        case KeyRelease:
        {
            keysState[event.xkey.keycode] = 0;
            for (size_t i = 0; i < keyListeners->count; i++)
                ((drawableObject_t*) keyListeners->collection[i])->keyEventFunc(
                        keyListeners->collection[i],
                        event.xkey.keycode,
                        KEY_RELEASE);
        }
            break;
        case ButtonPress:
        {
            mouseState[event.xbutton.button] = 1;
            for (size_t i = 0; i < mouseListeners->count; i++)
                ((drawableObject_t*) mouseListeners->collection[i])->mouseEventFunc(
                        mouseListeners->collection[i],
                        event.xbutton.x,
                        event.xbutton.y,
                        MOUSE_PRESSED,
                        event.xbutton.button);
        }
            break;
        case ButtonRelease:
        {
            mouseState[event.xbutton.button] = 0;
            for (size_t i = 0; i < mouseListeners->count; i++)
                ((drawableObject_t*) mouseListeners->collection[i])->mouseEventFunc(
                        mouseListeners->collection[i],
                        event.xbutton.x,
                        event.xbutton.y,
                        MOUSE_RELEASE,
                        event.xbutton.button);
        }
            break;
        case MotionNotify:
        {
            mouseX = event.xbutton.x;
            mouseY = event.xbutton.y;
            for (size_t i = 0; i < mouseMoveListeners->count; i++)
                ((drawableObject_t*) mouseMoveListeners->collection[i])->mouseMoveEventFunc(
                        mouseMoveListeners->collection[i],
                        event.xbutton.x,
                        event.xbutton.y);
        }
            break;
        case Expose:
            break;
    }
}

void u_clearObjects(bool free)
{
    if(free)
    {
        for(size_t i = 0; i < objects->count; i++)
            o_free((drawableObject_t*) objects->collection[i]);
    }

    objects->count = 0;
    mouseListeners->count = 0;
    keyListeners->count = 0;
    updateListeners->count = 0;
    mouseMoveListeners->count = 0;
}

void u_pushObject(drawableObject_t* object)
{
    listPush(objects, object);
    if(object->mouseEventFunc)
        listPush(mouseListeners, object);
    if(object->keyEventFunc)
        listPush(keyListeners, object);
    if(object->updateFunc)
        listPush(updateListeners, object);
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

        printf("[updater.c]: FPS: %lf. Objects: %li\n", fps, objects->count);
    }
}

void u_init(void)
{
    objects = createList(OBJECT_COUNT_START);
    keyListeners = createList(OBJECT_COUNT_START);
    mouseListeners = createList(OBJECT_COUNT_START);
    mouseMoveListeners = createList(OBJECT_COUNT_START);
    updateListeners = createList(OBJECT_COUNT_START);
}

void u_startLoop(winInfo_t* win)
{
    XEvent event;
    XSelectInput(win->display, win->win,
                 KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | ButtonMotionMask | ExposureMask | VisibilityChangeMask |
                 ResizeRedirectMask);

    defaultWin = win;
    while (1)
    {
        if(closed) break;

        if(XPending(win->display) == 0)
        {
            for(size_t i = 0; i < updateListeners->count; i++)
                ((drawableObject_t*)updateListeners->collection[i])->updateFunc(updateListeners->collection[i]);

            measureTime();
            continue;
        }

        XNextEvent(win->display, &event);
        eventHandler(event);
    }

}

void u_free()
{
    listFree(objects);
    listFree(keyListeners);
    listFree(mouseListeners);
    listFree(mouseMoveListeners);
    listFree(updateListeners);
}