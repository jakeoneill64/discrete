//
// Created by Jake M O'Neill on 14/09/2022.
//



#include "GreenBlock.h"
#include <glm/glm.hpp>

const Mesh& GreenBlock::getMesh() {


    return m_mesh;

}

GreenBlock::GreenBlock(){

    const Vertex vertices[8]{
            glm::vec3 {-0.5f, -0.5f, -0.5f}, glm::vec3 {-0.5f, -0.5f, 0.5f},
            glm::vec3 {0.5f, -0.5f, -0.5f},glm::vec3 {-0.5f, 0.5f, -0.5f},
            glm::vec3 {0.5f, 0.5f, -0.5f}, glm::vec3 {0.5f, -0.5f, 0.5f},
            glm::vec3 {-0.5f, 0.5f, 0.5f}, glm::vec3 {0.5f, 0.5f, 0.5f}
    };


    const unsigned int indices[36]{

            0, 2, 3,
            4, 2, 3,
            2, 5, 4,
            4, 7, 5,
            1, 7, 5,
            1, 7, 6,
            0, 6, 3,
            0, 6, 1,
            0, 5, 2,
            0, 5, 1,
            3, 7, 6,
            3, 7, 4


    };

    std::move(vertices, vertices + 8, std::back_inserter(m_mesh.vertices));
    std::move(indices, indices + 36, std::back_inserter(m_mesh.indices));

}

