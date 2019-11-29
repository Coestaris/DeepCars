//
// Created by maxim on 11/18/19.
//

#ifdef __GNUC__
#pragma implementation "light.h"
#endif
#include "light.h"

#define L_LOG(format, ...) DC_LOG("light.c", format, __VA_ARGS__)
#define L_ERROR(format, ...) DC_ERROR("light.c", format, __VA_ARGS__)

light_t* l_create()
{}

void l_free(light_t* light)
{}