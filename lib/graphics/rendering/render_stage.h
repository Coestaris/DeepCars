//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_STAGE_H
#define DEEPCARS_RENDER_STAGE_H

#include "../../object.h"
#include "../../shaders/shader.h"

typedef struct _render_stage {

   bool render_geometry;
   list_t* rendering_stages;
   shader_t* shader;

   void (*setup_obj_shader)(object_t* render_obj);
   void (*bind_shader)();
   void (*unbind_shader)();

   void* data;

} render_stage_t;


#endif //DEEPCARS_RENDER_STAGE_H
