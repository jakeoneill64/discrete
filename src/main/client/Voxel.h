//
// Created by Jake M O'Neill on 14/09/2022.
//

#ifndef MRONEILLSCUBES_VOXEL_H
#define MRONEILLSCUBES_VOXEL_H

#include "render/Mesh.h"
#include <glm/glm.hpp>

class Voxel{
public:

    constexpr static double size = 0.5f;
    virtual const Mesh* getMesh() = 0;
    glm::vec3 position;

};

#endif //MRONEILLSCUBES_VOXEL_H
