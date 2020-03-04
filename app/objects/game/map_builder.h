//
// Created by maxim on 2/27/20.
//

#ifndef DEEPCARS_MAP_BUILDER_H
#define DEEPCARS_MAP_BUILDER_H

#include "../../../lib/resources/model.h"

#define WALL_HEIGHT 15
#define WALL_WIDTH 10
#define MIN_DIST 0.01

model_t* build_map_model(list_t* walls);

#endif //DEEPCARS_MAP_BUILDER_H
