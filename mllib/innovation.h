//
// Created by maxim on 2/4/20.
//

#ifndef DEEPCARS_MLLIB_INNOVATION_H
#define DEEPCARS_MLLIB_INNOVATION_H

#include <stddef.h>
#include "../lib/coredefs.h"

typedef size_t innovation_t;

innovation_t i_get(void);
void i_recalc(void* connections, size_t count);
void i_reset(void);

#endif //DEEPCARS_MLLIB_INNOVATION_H
