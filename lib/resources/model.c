//
// Created by maxim on 9/11/19.
//

#include "model.h"

sphere_t* m_sphere(int stackCount, int sectorCount, float radius)
{
    sphere_t* sphere = malloc(sizeof(sphere_t));
    sphere->count = stackCount * sectorCount + 2;
    sphere->data = malloc(sizeof(float) * sphere->count * 6);
    size_t dataIndex = 0;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    //float s, t;                                   // vertex texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            sphere->data[dataIndex++] = x;
            sphere->data[dataIndex++] = y;
            sphere->data[dataIndex++] = z;

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            sphere->data[dataIndex++] = nx;
            sphere->data[dataIndex++] = ny;
            sphere->data[dataIndex++] = nz;

            // vertex tex coord (s, t) range between [0, 1]
            //s = (float)j / sectorCount;
            //t = (float)i / stackCount;
            //texCoords.push_back(s);
            //texCoords.push_back(t);
        }
    }

    glGenVertexArrays(1, &sphere->VAO);
    glGenBuffers(1, &sphere->VBO);

    glBindVertexArray(sphere->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dataIndex, sphere->data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return sphere;
}

void m_draw_sphere(sphere_t* sphere)
{
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBO);
    glBindVertexArray(sphere->VAO);

    glDrawElements(GL_TRIANGLES, sphere->count, GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}