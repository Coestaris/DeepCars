#include "graphics/win.h"
#include "shaders/shaderMgr.h"
#include "updater.h"
#include "textures/texture.h"
#include "graphics/gmath.h"
#include "graphics/camera.h"
#include "graphics/model.h"

void w_printInfo()
{
    printf("[GL Spec]: Vendor: %s\n", (char*) glGetString(GL_VENDOR));
    printf("[GL Spec]: Using OpenGL Version: %s\n", (char*) glGetString(GL_VERSION));
    printf("[GL Spec]: Using OpenGL Rendered: %s\n", (char*) glGetString(GL_RENDERER));
    printf("[GL Spec]: GLSH Version: %s\n", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}

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

unsigned int VBO, cubeVAO, lampVAO;

float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

shader_t* lampShader;
shader_t* shader;

texture_t* diffuse;
texture_t* specular;

mat4 projection;
mat4 view;
mat4 model;

vec4 cameraDirCpy;
vec4 cameraCrossCpy;
camera_t* camera;

sphere_t* sphere;
material_t* material;

vec4 lampPosition;
float angle;

void drawingRoutine()
{
    identityMat(view);

    cameraPitch = dy;
    cameraYaw = dx + M_PI / 2;

    if(cameraPitch >= M_PI / 2)
        cameraPitch = M_PI / 2 - 0.001f;
    if(cameraPitch <= - 3 * M_PI / 7)
        cameraPitch = - 3 * M_PI / 7 + 0.001;

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

    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(cubeVAO);

    sh_use(shader);
    identityMat(model);

   // rotateMat4Y(model, angle += 0.01);
    lampPosition[0] = cos(angle) * 5;
    lampPosition[2] = sin(angle+=0.06) * 5;

    sh_setMat4(shader, "projection", projection);
    sh_setMat4(shader, "view", view);

    sh_setVec3v(shader, "viewPos", camera->position[0], camera->position[1], camera->position[2]);
    sh_setMaterial(shader, material);

    sh_setVec3v(shader, "light.ambient",  0.1f, 0.1f, 0.1f);
    sh_setVec3v(shader, "light.diffuse",  0.5f, 0.5f, 0.5f);
    sh_setVec3v(shader, "light.specular", 1.0f, 1.0f, 1.0f);
    sh_setVec3v(shader, "light.position",  lampPosition[0], lampPosition[1], lampPosition[2]);

    identityMat(model);
    translateMat(model, -1, 0, -1);
    sh_setMat4(shader, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    identityMat(model);
    translateMat(model, -1, 0, 1);
    sh_setMat4(shader, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    identityMat(model);
    translateMat(model, 1, 0, -1);
    sh_setMat4(shader, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    identityMat(model);
    translateMat(model, 1, 0, 1);
    sh_setMat4(shader, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    glBindVertexArray(lampVAO);
    sh_use(lampShader);
    identityMat(model);
    translateMat(model,
            lampPosition[0],
            lampPosition[1],
            lampPosition[2]);
    scaleMat(model, .3f, .3f, .3f);

    sh_setMat4(lampShader, "projection", projection);
    sh_setMat4(lampShader, "view", view);
    sh_setMat4(lampShader, "model", model);
    sh_setVec3v(lampShader, "lightColor", 1, 1, 1);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    w_swapBuffers(win);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void initTriangle()
{
    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lampVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(lampVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    s_push(sh_create(  "../shaders/shaders/cube.vsh",
                       "../shaders/shaders/cube.fsh"), 1);
    s_push(sh_create(  "../shaders/shaders/lamp.vsh",
                       "../shaders/shaders/lamp.fsh"), 2);
    shader = s_getShader(1);
    lampShader = s_getShader(2);

    diffuse = t_create("image.png", vec2f(0, 0));
    t_load(diffuse);
    specular = t_create("specular.png", vec2f(0, 0));
    t_load(specular);

    sh_info(shader);

    float angleOfView = 60;
    float near = 0.1;
    float far = 100;
    float imageAspectRatio = win->w / (float)win->h;

    projection = cmat4();
    perspectiveFovMat(projection, angleOfView, imageAspectRatio, near, far);

    view = cmat4();
    model = cmat4();

    printMat4(projection);
    printMat4(view);
    printMat4(model);

    camera = c_create(cvec4(0, 0, 0, 0), cvec4(0, 1, 0, 0));
    camera->direction = cvec4(0, 0, 0, 0);

    cameraDirCpy = cvec4(0, 0, 0, 0);
    cameraCrossCpy = cvec4(0, 0, 0, 0);

    lampPosition = cvec4(5, 3, 3, 0);

    //sphere = m_sphere(10, 10, 4);
    material = mt_create(diffuse, specular, 128.0f);
}

void freeTriangle(void)
{
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lampVAO);
    glDeleteBuffers(1, &VBO);
}

int main(int argc, char* argv[])
{
    win = w_create(1200, 800, 0, 0, "hello world", false, stdout);
    w_printInfo();

    s_init();
    initTriangle();

    u_startLoop(win, drawingRoutine, proceedEvent);

    freeTriangle();
    s_free();

    w_destroy(win);
}