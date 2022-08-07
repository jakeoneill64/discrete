//
// Created by Jake M O'Neill on 06/08/2022.
//

#ifndef MRONEILLSCUBES_GLCONFIG_H
#define MRONEILLSCUBES_GLCONFIG_H

#include <cinttypes>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class GLConfig{
public:
    static uint32_t getMajorVersion();
    static uint32_t getMinorVersion();
    static uint32_t getProfile();
    static uint32_t getForwardCompatible();
private:
    static constexpr uint32_t majorVersion = uint32_t {3};
    static constexpr uint32_t minorVersion = uint32_t {3};
    static constexpr uint32_t profile = uint32_t{GLFW_OPENGL_CORE_PROFILE};
    static constexpr uint32_t forwardCompatible = uint32_t {GL_TRUE};
};

#endif //MRONEILLSCUBES_GLCONFIG_H
