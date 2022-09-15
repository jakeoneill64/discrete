//
// Created by Jake M O'Neill on 14/09/2022.
//

#ifndef MRONEILLSCUBES_MESH_H
#define MRONEILLSCUBES_MESH_H

#include "Vertex.h"

struct Mesh{

    const Vertex* const vertices;
    explicit Mesh(const Vertex* vertices);

};


#endif //MRONEILLSCUBES_MESH_H
