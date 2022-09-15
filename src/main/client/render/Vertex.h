//
// Created by Jake M O'Neill on 14/09/2022.
//

#ifndef MRONEILLSCUBES_VERTEX_H
#define MRONEILLSCUBES_VERTEX_H

#include <glm/glm.hpp>

struct Vertex{
    const glm::vec3 position;
    explicit Vertex(glm::vec3 position);
};


#endif //MRONEILLSCUBES_VERTEX_H
