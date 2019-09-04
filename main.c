#include "graphics/win.h"
#include "shaders/shaderMgr.h"
#include "updater.h"
#include "textures/texture.h"
#include "gmath.h"
#include "map/map.h"

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
            u_close();
            break;
        case Expose:
            break;
    }
}

unsigned int VBO, VAO, EBO;

float vertices[] = {
        // positions               // texture coords
        500.0f,  0.0f  ,   0.0f,  1.0f, 1.0f,   // top right
        500.0f,  600.0f,   0.0f,  1.0f, 0.0f,   // bottom right
        0.0f,  600.0f,   0.0f,    0.0f, 0.0f,   // bottom left
        0.0f,  0.0f  ,   0.0f,    0.0f, 1.0f    // top left
};

unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

shader_t* triangleSh;
texture_t* tex;
mat4 projection;
mat4 transform;
map_t* map;

float mixLevel = 0;

void drawingRoutine()
{
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    sh_use(triangleSh);
    t_bind(tex);

    sh_setMat4(triangleSh, "projection", projection);
    sh_setInt(triangleSh, "img", 0);

    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time

    w_swapBuffers(win);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void initTriangle()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

    map = m_load("level1");
    if(!map)
    {
        abort();
    }

    float r = win->w / 2;
    float t = win->h / 2;
    float n = -1;
    float f = 1;

    projection = cmat4();
    orthoMath(projection, n, f, r, t);

    transform = cmat4();
    identityMat(transform);
    translateMat(transform, -r, -t ,0);

    printMat4(projection);
    printMat4(transform);

    mat4_mulm(projection, transform);

}

void freeTriangle(void)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

int main(int argc, char* argv[])
{
    win = w_create(500, 600, 0, 0, "hello world", false, stdout);
    w_printInfo();

    s_init();
    initTriangle();

    u_startLoop(win, drawingRoutine, proceedEvent);

    freeTriangle();
    s_free();

    w_destroy(win);
}