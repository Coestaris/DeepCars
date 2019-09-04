//
// Created by maxim on 9/4/19.
//

#include "map.h"

#define readValue(descr, message, ptr)          \
    if(fscanf(file, descr, &ptr) != 1)          \
    {                                           \
        printf("Unable to read "message);       \
        fclose(file);                           \
        return NULL;                            \
    }

map_t* m_load(const char* fileName)
{
    map_t* result = malloc(sizeof(map_t));

    FILE* file = fopen(fileName, "r");
    if(!file)
    {
        printf("Unable to open file \"%s\"", fileName);
        return NULL;
    }

    readValue("%li", "sectors count", result->sectorsCount);

    result->sectors = malloc(sizeof(map_sector_t*) * result->sectorsCount);
    for(size_t i = 0; i < result->sectorsCount; i++)
    {
        result->sectors[i] = malloc(sizeof(map_sector_t));
        readValue("%lf", "x1", result->sectors[i]->a.x);
        readValue("%lf", "y1", result->sectors[i]->a.y);
        readValue("%lf", "x2", result->sectors[i]->b.x);
        readValue("%lf", "y2", result->sectors[i]->b.y);
        readValue("%lf", "x3", result->sectors[i]->c.x);
        readValue("%lf", "y3", result->sectors[i]->c.y);
        readValue("%lf", "x4", result->sectors[i]->d.x);
        readValue("%lf", "y4", result->sectors[i]->d.y);
        readValue("%lf", "y4", result->sectors[i]->height);
    }

    fclose(file);
    return result;
}