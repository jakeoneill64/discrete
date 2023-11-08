//
// Created by Jake M O'Neill on 05/11/2023.
//

#ifndef DISCRETE_SPACE_H
#define DISCRETE_SPACE_H

#include <unordered_map>
#include "voxel.h"

class Space{

private:

    std::unordered_map<Voxel, VoxelProperties> propertiesByVoxel;

};

#endif //DISCRETE_SPACE_H
