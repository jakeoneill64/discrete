#ifndef DISCRETE_ENTITY_H
#define DISCRETE_ENTITY_H

#include <cinttypes>
#include <glm/glm.hpp>

class Entity{
public:
    const uint32_t id;
    const glm::vec3 position;
    const glm::vec3 lookAt;
};

#endif

