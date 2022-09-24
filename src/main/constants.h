//
// Created by Jake M O'Neill on 18/09/2022.
//

#ifndef DISCRETE_CONSTANTS_H
#define DISCRETE_CONSTANTS_H

#include <string>
#include "GLFW/glfw3.h"
#include "client/render/Vertex.h"

namespace discrete{

    constexpr const char* ENGINE_NAME = "discrete";
    constexpr const char* CLIENT_LOGGER_NAME = "discrete-client";
    constexpr const char* SERVER_LOGGER_NAME = "discrete-server";

    //The versions are not hard constraints but signify a bare minimum.
    constexpr unsigned int GL_MAJOR = 3;
    constexpr unsigned int GL_MINOR = 3;
    constexpr unsigned int GL_INDEX_BUFFER_SIZE = sizeof(Vertex) * 5 * 1024;
    constexpr unsigned int GL_ELEMENT_BUFFER_SIZE = sizeof(unsigned int) * 5 * 1536; //ie 3/2 * element buffer indices per vertex

    constexpr unsigned int WINDOW_START_WIDTH = 800;
    constexpr unsigned int WINDOW_START_HEIGHT = 600;

};

#endif
