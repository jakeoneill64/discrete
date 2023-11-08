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
        boost::hash_combine(result, static_cast<long double>(vector.i));
        boost::hash_combine(result, static_cast<long double>(vector.j));
        boost::hash_combine(result, static_cast<long double>(vector.k));
        return result;
    }
};

#endif //DISCRETE_TYPES_H
