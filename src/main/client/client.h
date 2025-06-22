#ifndef CLIENTS_H
#define CLIENTS_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


struct DestroyGLFWWindow{

    void operator()(GLFWwindow* ptr) const;

};

void client_loop();

#endif //CLIENTS_H
