//
// Created by Jake M O'Neill on 14/09/2022.
//



#include "GreenBlock.h"

const Mesh* GreenBlock::getMesh() {

    const Vertex* vertices = new Vertex{glm::vec3 {0.0f, 0.0f, 0.0f}};
    const Mesh* mesh = new Mesh(vertices);
    return mesh;

}

