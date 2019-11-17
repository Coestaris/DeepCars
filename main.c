#include "lib/graphics/win.h"
#include "lib/shaders/shaderMgr.h"
#include "lib/updater.h"
#include "lib/resources/model.h"
#include "lib/graphics/graphics.h"

winInfo_t* win;

int keysState[256];
int mouseState[10];

int mousePressX[10];
int mousePressY[10];

float cameraPitch = 0;
float cameraYaw = M_PI / 2;

float dx;
float dy;

float lastdx = 0;
float lastdy = 0;

void proceedEvent(XEvent event)
{
    switch(event.type)
    {
        case KeyPress:
            keysState[event.xkey.keycode] = 1;
            break;
        case KeyRelease:
            keysState[event.xkey.keycode] = 0;
            if(event.xkey.keycode == 9)
                u_close();
            break;
        case ButtonPress:
            mouseState[event.xbutton.button] = 1;
            mousePressX[event.xbutton.button] = event.xbutton.x;
            mousePressY[event.xbutton.button] = event.xbutton.y;
            break;
        case ButtonRelease:
            mouseState[event.xbutton.button] = 0;
            lastdx = dx;
            lastdy = dy;
            break;
        case MotionNotify:
            if(mouseState[1])
            {
                dx = ((float)event.xmotion.x - (float)mousePressX[1]) / 100.0f + lastdx;
                dy = ((float)event.xmotion.y - (float)mousePressY[1]) / 100.0f + lastdy;
            }
            break;
        case Expose:
            break;
    }
}

mat4 view;
mat4 model;
vec4 cameraDirCpy;
vec4 cameraCrossCpy;
camera_t* camera;

model_t* m;

void drawingRoutine()
{
    cameraPitch = dy;
    cameraYaw = dx + M_PI / 2;
    identityMat(view);

    c_rotate(camera, cameraPitch, cameraYaw);
    c_toMat(view, camera);

    vec4_cpy(cameraDirCpy, camera->direction);
    vec4_mulf(cameraDirCpy, .2);

    vec4_cpy(cameraCrossCpy, camera->_cameraRight);
    vec4_mulf(cameraCrossCpy, .2);

    //left
    if(keysState[38])
        vec4_subv(camera->position, cameraCrossCpy);

    //right
    if(keysState[40])
        vec4_addv(camera->position, cameraCrossCpy);

    //forward
    if(keysState[25])
        vec4_subv(camera->position, cameraDirCpy);

    //back
    if(keysState[39])
        vec4_addv(camera->position, cameraDirCpy);

    //up
    if(keysState[65])
        camera->position[1] += 0.2f;
    //down
    if(keysState[50])
        camera->position[1] -= 0.2f;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gr_draw_model_simpleColor(m, COLOR_AQUA);

    w_swapBuffers(win);
}

void init()
{
    float angleOfView = 60;
    float near = 0.1;
    float far = 100;
    float imageAspectRatio = win->w / (float)win->h;

    win->projection = cmat4();
    perspectiveFovMat(win->projection, angleOfView, imageAspectRatio, near, far);

    view = cmat4();
    model = cmat4();

    printMat4(win->projection);
    printMat4(view);
    printMat4(model);

    camera = c_create(cvec4(0, 0, 0, 0), cvec4(0, 1, 0, 0));
    camera->direction = cvec4(0, 0, 0, 0);

    cameraDirCpy = cvec4(0, 0, 0, 0);
    cameraCrossCpy = cvec4(0, 0, 0, 0);

    m = m_load("cube.obj");
    m_build(m);
}


int main(int argc, char* argv[])
{
    win = w_create(1200, 800, 0, 0, "hello world", false, stdout);
    w_printInfo();

    s_init();
    init();

    gr_init(win->projection, view);

    u_startLoop(win, drawingRoutine, proceedEvent);
    s_free();

    w_destroy(win);
}