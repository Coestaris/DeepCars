//
// Created by maxim on 9/11/19.
//

#include "model.h"

model_t* m_load(const char* filename)
{
    FILE* f = fopen(filename, "r");
    if(!f)
    {
        printf("Unable to open file \"%s\"", filename);
        return NULL;
    }

    fseek(f, SEEK_END, 0);
    size_t size = ftell(f);
    fseek(f, SEEK_SET, 0);

    char* data = malloc(sizeof(f));
    fread(data, size, 1, f);

    model_t* model = malloc(sizeof(model_t));

    free(data);
    fclose(f);
    return model;
}

void m_free(model_t* model)
{

}