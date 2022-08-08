//
// Created by Jake M O'Neill on 06/08/2022.
//

#ifndef MRONEILLSCUBES_RENDERCONFIGURATION_H
#define MRONEILLSCUBES_RENDERCONFIGURATION_H

#include <cinttypes>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>


// TODO overhaul this temp class.
class RenderConfiguration{

public :
    static constexpr uint32_t glMajorVersion = uint32_t {3};
    static constexpr uint32_t glMinorVersion = uint32_t {3};
    static constexpr uint32_t glProfile = uint32_t{GLFW_OPENGL_CORE_PROFILE};
    static constexpr uint32_t glForwardCompatible = uint32_t {GL_TRUE};
    static constexpr uint16_t windowStartWidth = uint16_t {800};
    static constexpr uint16_t windowStartHeight = uint16_t {600};
    static constexpr char *const windowName = "";

};

#endif //MRONEILLSCUBES_RENDERCONFIGURATION_H
