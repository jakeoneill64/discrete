//
// Created by Jake M O'Neill on 18/09/2022.
//

#ifndef DISCRETE_CONSTANTS_H
#define DISCRETE_CONSTANTS_H

#include <string>
#include "GLFW/glfw3.h"

namespace discrete{

    constexpr const char* ENGINE_NAME = "discrete";
    constexpr const char* CLIENT_LOGGER_NAME = "discrete-client";
    constexpr const char* SERVER_LOGGER_NAME = "discrete-server";
    constexpr const char* DEFAULT_CLIENT_CONFIG_FILE = "default-client-config.dat";
    constexpr const char* CLIENT_CONFIG_FILE = ".discrete/client-config.json";
    constexpr const char* WORLD_DATA_DIR = ".discrete/world/";

    constexpr unsigned int GL_ELEMENT_BUFFER_SIZE = sizeof(unsigned int) * 5 * 1536; //ie 3/2 * element buffer indices per vertex


};

#endif
