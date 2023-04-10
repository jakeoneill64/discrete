//
// Created by Jake M O'Neill on 14/09/2022.
//

#ifndef MRONEILLSCUBES_MESH_H
#define MRONEILLSCUBES_MESH_H

#include "Vertex.h"
#include <vector>
#include <glm/glm.hpp>


namespace render {

    struct Vertex{
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct Mesh {

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

    };

    const Mesh& cube_mesh();

}






#endif //MRONEILLSCUBES_MESH_H
