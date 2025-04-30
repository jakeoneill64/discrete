#ifndef DISCRETE_MESH_H
#define DISCRETE_MESH_H

#include "glm/glm.hpp"
#include "engine/World.h"
#include <functional>
#include <memory>
#include <vector>
#include <ranges>

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
};


glm::vec3 vertices[] = {
    // Front face
    {-0.5f, -0.5f,  0.5f},  // 0
    { 0.5f, -0.5f,  0.5f},  // 1
    { 0.5f,  0.5f,  0.5f},  // 2
    {-0.5f,  0.5f,  0.5f},  // 3

    // Back face
    {-0.5f, -0.5f, -0.5f},  // 4
    { 0.5f, -0.5f, -0.5f},  // 5
    { 0.5f,  0.5f, -0.5f},  // 6
    {-0.5f,  0.5f, -0.5f},  // 7

    // Top face
    {-0.5f,  0.5f,  0.5f},  // 8
    { 0.5f,  0.5f,  0.5f},  // 9
    { 0.5f,  0.5f, -0.5f},  // 10
    {-0.5f,  0.5f, -0.5f},  // 11

    // Bottom face
    {-0.5f, -0.5f,  0.5f},  // 12
    { 0.5f, -0.5f,  0.5f},  // 13
    { 0.5f, -0.5f, -0.5f},  // 14
    {-0.5f, -0.5f, -0.5f},  // 15

    // Right face
    { 0.5f, -0.5f,  0.5f},  // 16
    { 0.5f,  0.5f,  0.5f},  // 17
    { 0.5f,  0.5f, -0.5f},  // 18
    { 0.5f, -0.5f, -0.5f},  // 19

    // Left face
    {-0.5f, -0.5f,  0.5f},  // 20
    {-0.5f,  0.5f,  0.5f},  // 21
    {-0.5f,  0.5f, -0.5f},  // 22
    {-0.5f, -0.5f, -0.5f}   // 23
};

glm::vec3 normals[] = {
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},

    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},

    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},

    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},

    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},

    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f}
};


#endif //MRONEILLSCUBES_MESH_H
