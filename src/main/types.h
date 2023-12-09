//
// Created by Jake M O'Neill on 05/11/2023.
//

#ifndef DISCRETE_TYPES_H
#define DISCRETE_TYPES_H
#include "util/fixed_point.h"
#include "boost/functional/hash.hpp"

using vec3 = struct {
    fixed64_mid i, j, k;
};

template<>
struct std::hash<vec3>{
    size_t operator()(const vec3& vector){
        size_t result{0};
        static std::hash<fixed64_mid> fixed_hasher;
        boost::hash_combine(result, fixed_hasher(vector.i));
        boost::hash_combine(result, fixed_hasher(vector.j));
        boost::hash_combine(result, fixed_hasher(vector.k));
        return result;
    }
};

#endif //DISCRETE_TYPES_H
