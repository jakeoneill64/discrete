#ifndef DISCRETE_VOXEL_H
#define DISCRETE_VOXEL_H

#include "glm/glm.hpp"
#include "util/fixed_point.h"
#include "util/Colour.h"
#include "Material.h"
#include <memory>

struct Voxel{
    glm::vec<3, fixed64_mid> position;
    glm::vec<3, fixed64_mid> velocity;
    fixed32_mid diameter;
    Color emission;
    const Material& material;
};

//template<>
//struct std::hash<Voxel> {
//
//    size_t operator()(const Voxel &voxel) const {
//        size_t result{0};
//        static std::hash<vec3> vector_hasher;
//        static std::hash<fixed32_mid> fixed_point_hasher;
//        boost::hash_combine(result, fixed_point_hasher(voxel.diameter));
//        boost::hash_combine(result, vector_hasher(voxel.position));
//        boost::hash_combine(result, vector_hasher(voxel.velocity));
//        return result;
//    }
//
//};

#endif //DISCRETE_VOXEL_H
