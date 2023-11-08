//
// Created by Jake M O'Neill on 05/11/2023.
//

#include "glm/vec3.hpp"
#include "types.h"
#include <boost/functional/hash.hpp>

#ifndef DISCRETE_VOXEL_H
#define DISCRETE_VOXEL_H

struct VoxelProperties{
    fixed32_high frictionCoefficient;
    fixed32_high density;
};

struct Voxel{
    vec3 position;
    vec3 velocity;
    ufixed32_mid diameter;
};

template<>
struct std::hash<Voxel> {

    size_t operator()(const Voxel &voxel) const {
        size_t result{0};
        boost::hash_combine(result, static_cast<double>(voxel.diameter));
        boost::hash_combine(result, voxel.position);
        boost::hash_combine(result, voxel.velocity);
        return result;
    }

};

#endif //DISCRETE_VOXEL_H
