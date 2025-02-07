#ifndef DISCRETE_VOXEL_H
#define DISCRETE_VOXEL_H

#include "glm/vec3.hpp"
#include <boost/functional/hash.hpp>
#include "util/vector.h"
#include "util/fixed_point.h"

using namespace discrete;

struct Voxel{
    vec3 position;
    vec3 velocity;
    fixed32_mid diameter;
    fixed32_high frictionCoefficient;
    fixed32_high density;
    fixed32_high restitutionCoefficient;
};

template<>
struct std::hash<Voxel> {

    size_t operator()(const Voxel &voxel) const {
        size_t result{0};
        static std::hash<vec3> vector_hasher;
        static std::hash<fixed32_mid> fixed_point_hasher;
        boost::hash_combine(result, fixed_point_hasher(voxel.diameter));
        boost::hash_combine(result, vector_hasher(voxel.position));
        boost::hash_combine(result, vector_hasher(voxel.velocity));
        return result;
    }

};

#endif //DISCRETE_VOXEL_H
