//
// Created by Jake M O'Neill on 14/09/2022.
//

#ifndef MRONEILLSCUBES_GREENBLOCK_H
#define MRONEILLSCUBES_GREENBLOCK_H

#include "client/Voxel.h"
#include "Mesh.h"

class GreenBlock: public Voxel{
public:
    const Mesh& getMesh() override;
    GreenBlock();
};

#endif //MRONEILLSCUBES_GREENBLOCK_H
