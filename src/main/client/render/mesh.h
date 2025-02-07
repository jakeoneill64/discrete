#ifndef DISCRETE_MESH_H
#define DISCRETE_MESH_H

#include "glm/glm.hpp"
#include "engine/World.h"
#include "util/vector.h"
#include <functional>
#include <memory>
#include <vector>
#include <ranges>

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
};

struct Mesh {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

};


#endif //MRONEILLSCUBES_MESH_H
