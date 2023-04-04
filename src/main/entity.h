//
// Created by Jake M O'Neill on 01/10/2022.
//

#ifndef MRONEILLSCUBES_ENTITY_H
#define MRONEILLSCUBES_ENTITY_H

#include <glm/glm.hpp>
#include <mutex>

class Entity{
public:
    const u_int32_t id;
    void updatePosition(const glm::vec3& movement);
    [[nodiscard]] glm::mat4 getViewMatrix() const;
private:
    glm::vec3 m_position;
    glm::vec3 m_targetPosition;
    std::mutex m_mutex;
};


struct EntityMovement{
    uint32_t entityId;
    glm::vec3 movement;
};

#endif //MRONEILLSCUBES_ENTITY_H
