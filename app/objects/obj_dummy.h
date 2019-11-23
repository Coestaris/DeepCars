//
// Created by maxim on 11/18/19.
//

#ifndef DEEPCARS_OBJ_DUMMY_H
#define DEEPCARS_OBJ_DUMMY_H

#include "../../lib/object_include.h"
#include "../win_defaults.h"

object_t* create_dummy(vec3f_t pos, float size, vec4 color, model_t* model);
object_t* create_textured_dummy(vec3f_t pos, float size, texture_t* texture, model_t* model);

#endif //DEEPCARS_OBJ_DUMMY_H
