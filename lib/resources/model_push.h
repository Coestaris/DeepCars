//
// Created by maxim on 2/27/20.
//

#ifndef DEEPCARS_MODEL_PUSH_H
#define DEEPCARS_MODEL_PUSH_H

#include "model.h"

// Push some data to a model. It's index will be automatically incremented.
// If it doesn't fit in existing array it will be resized to fit new element
void m_push_vertex(model_t* model, vec4 vec);
void m_push_normal(model_t* model, vec4 vec);
void m_push_tex_coord(model_t* model, vec4 vec);
void m_push_face(model_t* model, model_face_t* face);
void m_push_group_name(model_t* model, char* group_name);
void m_push_mtllib(model_t* model, char* mtllib);
void m_push_used_material(model_t* model, material_t* used_material);

#endif //DEEPCARS_MODEL_PUSH_H
