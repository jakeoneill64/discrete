#ifndef MRONEILLSCUBES_CAMERA_H
#define MRONEILLSCUBES_CAMERA_H

#include "glm/glm.hpp"


class Camera{
public:

    glm::mat4 getViewMatrix() const;
    glm::vec3 m_position;
    glm::vec3 m_targetPosition;

};

#endif //MRONEILLSCUBES_CAMERA_H
