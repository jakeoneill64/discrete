//
// Created by Jake M O'Neill on 05/11/2023.
//

#include "glm/vec3.hpp"
#include <boost/functional/hash.hpp>
#include "util/vector.h"

#ifndef DISCRETE_VOXEL_H
#define DISCRETE_VOXEL_H

struct Voxel{
    discrete::vec3 position;
    discrete::vec3 velocity;
    fixed32_mid diameter;
    fixed32_high frictionCoefficient;
    fixed32_high density;
};

template<>
struct std::hash<Voxel> {

    size_t operator()(const Voxel &voxel) const {
        size_t result{0};
        static std::hash<discrete::vec3> vector_hasher;
        static std::hash<fixed32_mid> fixed_point_hasher;
        boost::hash_combine(result, fixed_point_hasher(voxel.diameter));
        boost::hash_combine(result, vector_hasher(voxel.position));
        boost::hash_combine(result, vector_hasher(voxel.velocity));
        return result;
    }

};

#endif //DISCRETE_VOXEL_H
