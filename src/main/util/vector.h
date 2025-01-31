#ifndef DISCRETE_VECTOR_H
#define DISCRETE_VECTOR_H

#include "fixed_point.h"

// template this maybe? don't really like that it is conflicted with glm.
struct vec3 {
    fixed64_mid i, j, k;
};

struct vec2 {
    fixed64_mid i, j;
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


#endif //DISCRETE_VECTOR_H
