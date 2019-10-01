//
// Created by maxim on 9/11/19.
//

#include "model.h"

void push_vertex()
{

}

void push_normal()
{

}

void push_face()
{

}

void push_texture()
{

}

const char* allowedDescriptors[] =
{
    "v",
    "vn",
    "vt",
    "f",

    "g",
    "o",

    "mtllib",
    "usemtl",

    "#",
};

bool isSeparator(char sym)
{
    return (sym == ' ' || sym == '\n' || sym == '\t' || sym == '\0');
}

void readToSeparator(char* string, size_t position, size_t* resPos)
{
    size_t strLen = strlen(string);
    while(!isSeparator(string[position]) && position < strLen) position++;
    *resPos = position;
}

model_t* m_load(const char* filename)
{
    //sizeof(allowedDescriptors) / sizeof(const char*)
    FILE* f = fopen(filename, "r");
    if(!f)
    {
        printf("Unable to open file \"%s\"", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = malloc(size + 1);
    memset(data, 0, size + 1);

    fread(data, size, 1, f);

    model_t* model = malloc(sizeof(model_t));

    bool comment = false;
    bool firstSymbol = false;

    size_t resPos = 0, pos = 0;
    while(resPos <= size - 1)
    {
        readToSeparator(data, pos, &resPos);
        for (size_t i = pos; i < resPos; i++)
            putchar(data[i]);
        puts("===");
        pos = resPos;
    }

    free(data);
    fclose(f);
    return model;
}

void m_free(model_t* model)
{

}