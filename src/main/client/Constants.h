//
// Created by Jake M O'Neill on 18/09/2022.
//

#include <string>
#include "GLFW/glfw3.h"

namespace discrete{

    constexpr const char* ENGINE_NAME = "discrete";

    //The versions are not hard constraints but signify a bare minimum.
    constexpr unsigned int GL_MAJOR = 3;
    constexpr unsigned int GL_MINOR = 3;
    constexpr unsigned int GL_PROFILE = GLFW_OPENGL_CORE_PROFILE;
    constexpr unsigned int GL_BUF_SIZE = 1024 * 1024 * 5;

    constexpr int WINDOW_START_WIDTH = 800;
    constexpr int WINDOW_START_HEIGHT = 600;

};

