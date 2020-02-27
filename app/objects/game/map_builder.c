//
// Created by maxim on 2/27/20.
//

#ifdef __GNUC__
#pragma implementation "map_builder.h"
#endif
#include "map_builder.h"
#include "../editor/map_saver.h"

#define WALL_HEIGHT 30
#define WALL_WIDTH 10

model_t* build_model(list_t* walls)
{
   for(size_t i = 0; i < walls->count; i++)
   {
      wall_t* wall = walls->collection[i];
      wall->p1
   }
}


