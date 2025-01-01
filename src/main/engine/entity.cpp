#include "entity.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"




//glm::mat4 Entity::getViewMatrix() const{
////    std::lock_guard lock{m_mutex};
//    glm::vec3 target = glm::normalize(m_position - m_targetPosition);
//    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//    glm::vec3 cameraUp = glm::cross(target, glm::normalize(glm::cross(up, target)));
//    return glm::lookAt(m_position, target, cameraUp);
//}
//
//void Entity::updatePosition(const glm::vec3& movement) {
//    std::lock_guard lock{m_mutex};
//    m_position += movement;
//}

EntityMoveState::EntityMoveState(const vec3 &direction) {

}

void EntityMoveState::operator()() const {

}