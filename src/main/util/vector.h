#ifndef DISCRETE_VECTOR_H
#define DISCRETE_VECTOR_H

#include "fixed_point.h"
#include "boost/functional/hash.hpp"

namespace discrete {

    struct vec3 {
        fixed64_mid i, j, k;
    };
};

template<>
struct std::hash<discrete::vec3>{
    size_t operator()(const discrete::vec3& vector){
        size_t result{0};
        static std::hash<fixed64_mid> fixed_hasher;
        boost::hash_combine(result, fixed_hasher(vector.i));
        boost::hash_combine(result, fixed_hasher(vector.j));
        boost::hash_combine(result, fixed_hasher(vector.k));
        return result;
    }
};


#endif //DISCRETE_VECTOR_H
