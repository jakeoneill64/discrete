//
// Created by Jake M O'Neill on 08/09/2022.
//

#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

glm::mat4 Camera::getLookAtMatrix() {
    glm::vec3 target = glm::normalize(*m_position - *m_targetPosition);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraUp = glm::cross(target, glm::normalize(glm::cross(up, target)));
    return glm::lookAt(*m_position, target, cameraUp);
}


