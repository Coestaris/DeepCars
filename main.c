#include "graphics/win.h"
#include "shaders/shaderMgr.h"
#include "updater.h"
#include "textures/texture.h"
#include "gmath.h"

void w_printInfo()
{
    printf("[GL Spec]: Vendor: %s\n", (char*) glGetString(GL_VENDOR));
    printf("[GL Spec]: Using OpenGL Version: %s\n", (char*) glGetString(GL_VERSION));
    printf("[GL Spec]: Using OpenGL Rendered: %s\n", (char*) glGetString(GL_RENDERER));
    printf("[GL Spec]: GLSH Version: %s\n", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}

winInfo_t* win;

vec4 cameraPosition;
vec4 cameraTarget;
vec4 cameraUpDirection;

int keysState[256];

void proceedEvent(XEvent event)
{
    switch(event.type)
    {
        case KeyPress:
            keysState[event.xkey.keycode] = 1;
            break;
        case KeyRelease:
            keysState[event.xkey.keycode] = 0;
            break;
        case ButtonPress:
            break;
        case ButtonRelease:
            //u_close();
            break;
        case Expose:
            break;
    }
}

unsigned int VBO, VAO;

float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

shader_t* triangleSh;
texture_t* tex;

mat4 projection;
mat4 view;
mat4 model;

float angle = 0;

mat4 xRotate;
mat4 yRotate;
mat4 zRotate;

float positions[10][3] = {
        {  0.0f,  0.0f,  0.0f },
        {  2.0f,  5.0f, -15.0f },
        { -1.5f, -2.2f, -2.5f },
        { -3.8f, -2.0f, -12.3f },
        {  2.4f, -0.4f, -3.5f },
        { -1.7f,  3.0f, -7.5f },
        {  1.3f, -2.0f, -2.5f },
        {  1.5f,  2.0f, -2.5f },
        {  1.5f,  0.2f, -1.5f },
        { -1.3f,  1.0f, -1.5 },
};

void drawingRoutine()
{
    //left
    if(keysState[38])
        cameraPosition[0] -= 0.01f;
    //right
    if(keysState[40])
        cameraPosition[0] += 0.01f;
    //forward
    if(keysState[25])
        cameraPosition[2] -= 0.01f;
    //back
    if(keysState[39])
        cameraPosition[2] += 0.01f;
    //up
    if(keysState[65])
        cameraPosition[1] += 0.01f;
    //down
    if(keysState[50])
        cameraPosition[1] -= 0.01f;

    vec4_cpy(cameraTarget, cameraPosition);
    cameraTarget[2] -= 10;

    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw our first triangle
    sh_use(triangleSh);
    t_bind(tex);

    identityMat(view);

    lookAtMat(view,
            cameraPosition,
            cameraTarget,
            cameraUpDirection);

    sh_setMat4(triangleSh, "projection", projection);
    sh_setMat4(triangleSh, "view", view);

    glBindVertexArray(VAO);

    sh_setInt(triangleSh, "img", 0);

    identityMat(xRotate);
    identityMat(yRotate);
    identityMat(zRotate);

    for(size_t i = 0; i < 10; i++)
    {
        identityMat(model);

        rotateMat4X(xRotate, 5 * i);
        rotateMat4Y(yRotate, 3 * i);
        rotateMat4Z(zRotate, 0);

        mat4_mulm(model, xRotate);
        mat4_mulm(model, yRotate);
        mat4_mulm(model, zRotate);

        translateMat(model,
                     positions[i][0],
                     positions[i][1],
                     positions[i][2]);

        sh_setMat4(triangleSh, "model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    w_swapBuffers(win);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void initTriangle()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    s_push(sh_create(  "../shaders/shaders/render.vsh",
                       "../shaders/shaders/render.fsh"), 1);
    triangleSh = s_getShader(1);

    tex = t_create("image.png", vec2f(0, 0));
    t_load(tex);

    sh_info(triangleSh);
    sh_setInt(triangleSh, "ourTexture", 0);

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

    xRotate = cmat4();
    yRotate = cmat4();
    zRotate = cmat4();

    lookAtInit();

    cameraPosition = cvec4(0, 0, 0, 0);
    cameraTarget = cvec4(0, 0, -5, 0);
    cameraUpDirection = cvec4(0, 1, 0, 0);
}

void freeTriangle(void)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

int main(int argc, char* argv[])
{
    win = w_create(800, 600, 0, 0, "hello world", false, stdout);
    w_printInfo();

    s_init();
    initTriangle();

    u_startLoop(win, drawingRoutine, proceedEvent);

    freeTriangle();
    s_free();

    w_destroy(win);
}