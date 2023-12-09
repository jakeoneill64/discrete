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
        static std::hash<vec3> vector_hasher;
        static std::hash<ufixed32_mid> fixed_point_hasher;
        boost::hash_combine(result, fixed_point_hasher(voxel.diameter));
        boost::hash_combine(result, vector_hasher(voxel.position));
        boost::hash_combine(result, vector_hasher(voxel.velocity));
        return result;
    }

};

#endif //DISCRETE_VOXEL_H
