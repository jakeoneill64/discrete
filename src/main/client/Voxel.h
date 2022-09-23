//
// Created by Jake M O'Neill on 14/09/2022.
//

#ifndef MRONEILLSCUBES_VOXEL_H
#define MRONEILLSCUBES_VOXEL_H

#include "render/Mesh.h"
#include <glm/glm.hpp>
#include <memory>

class Voxel{
public:

    virtual Mesh&& getMesh() = 0;
    glm::mat4 modelTransform;


protected:

    Mesh m_mesh;

};

#endif //MRONEILLSCUBES_VOXEL_H
