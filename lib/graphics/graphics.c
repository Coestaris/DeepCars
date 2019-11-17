//
// Created by maxim on 8/26/19.
//

#include "graphics.h"
#include "../shaders/shader.h"
#include "../shaders/shaderMgr.h"

vec4 COLOR_WHITE;
vec4 COLOR_SILVER;
vec4 COLOR_GRAY;
vec4 COLOR_BLACK;
vec4 COLOR_RED;
vec4 COLOR_MAROON;
vec4 COLOR_YELLOW;
vec4 COLOR_OLIVE;
vec4 COLOR_LIME;
vec4 COLOR_GREEN;
vec4 COLOR_AQUA;
vec4 COLOR_TEAL;
vec4 COLOR_BLUE;
vec4 COLOR_NAVY;
vec4 COLOR_FUCHSIA;
vec4 COLOR_PURPLE;

void gr_fill(vec4 color)
{
    glClearColor(color[0], color[1], color[2], color[3]);
}

shader_t* shader_simpleColored;
mat4 proj;
mat4 view;

void gr_init(mat4 _proj, mat4 _view)
{
    COLOR_WHITE   = cvec4(1, 1, 1, 0);
    COLOR_SILVER  = cvec4(.75, .75, .75, 0);
    COLOR_GRAY    = cvec4(.5, .5, .5, 0);
    COLOR_BLACK   = cvec4(0, 0, 0, 0);
    COLOR_RED     = cvec4(1, 0, 0, 0);
    COLOR_MAROON  = cvec4(.5, 0, 0, 0);
    COLOR_YELLOW  = cvec4(1, 1, 0, 0);
    COLOR_OLIVE   = cvec4(.5, .5, 0, 0);
    COLOR_LIME    = cvec4(0, 1, 0, 0);
    COLOR_GREEN   = cvec4(0, 0.5, 0, 0);
    COLOR_AQUA    = cvec4(0, 1, 1, 0);
    COLOR_TEAL    = cvec4(0, .5, .5, 0);
    COLOR_BLUE    = cvec4(0, 0, 1, 0);
    COLOR_NAVY    = cvec4(0, 0, .5, 0);
    COLOR_FUCHSIA = cvec4(1, 0, 1, 0);
    COLOR_PURPLE  = cvec4(.5, 0, .5, 0);

    shader_simpleColored = s_getShader(SH_SIMPLECOLORED);

    proj = _proj;
    view = _view;
}

void gr_free(void)
{
    freeVec4(COLOR_WHITE);
    freeVec4(COLOR_SILVER);
    freeVec4(COLOR_GRAY);
    freeVec4(COLOR_BLACK);
    freeVec4(COLOR_RED);
    freeVec4(COLOR_MAROON);
    freeVec4(COLOR_YELLOW);
    freeVec4(COLOR_OLIVE);
    freeVec4(COLOR_LIME);
    freeVec4(COLOR_GREEN);
    freeVec4(COLOR_AQUA);
    freeVec4(COLOR_TEAL);
    freeVec4(COLOR_BLUE);
    freeVec4(COLOR_NAVY);
    freeVec4(COLOR_FUCHSIA);
    freeVec4(COLOR_PURPLE);
}

void gr_draw_model(model_t* model)
{
    glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
    glBindVertexArray(model->VAO);

    glDrawArrays(GL_TRIANGLES, 0, model->modelLen->facesCount * 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void gr_draw_model_simpleColor(model_t* model, vec4 color)
{
    sh_use(shader_simpleColored);
    sh_setVec3v(shader_simpleColored,
            shader_simpleColored->uniformLocations[SH_SIMPLECOLORED_COLOR],
            color[0], color[1], color[2]);

    sh_setMat4(shader_simpleColored,
                shader_simpleColored->uniformLocations[SH_SIMPLECOLORED_PROJ],
                proj);

    sh_setMat4(shader_simpleColored,
               shader_simpleColored->uniformLocations[SH_SIMPLECOLORED_VIEW],
               view);

    sh_setMat4(shader_simpleColored,
               shader_simpleColored->uniformLocations[SH_SIMPLECOLORED_MODEL],
               model->model);

    //printMat4(proj);
    //printMat4(model->model);
    //printMat4(view);

    gr_draw_model(model);
}