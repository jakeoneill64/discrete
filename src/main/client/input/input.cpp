//
// Created by Jake M O'Neill on 24/09/2022.
//

#include "input.h"

using namespace discrete;

void EntityInputManager::onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_RELEASE) return;
    auto function = actionByKey.find(key);
    if(function != actionByKey.end()) function->second(window);
}

void EntityInputManager::onMouseEvent(GLFWwindow* window, double xpos, double ypos) {}

void EntityInputManager::onActivate(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


EntityInputManager::EntityInputManager() noexcept:
actionByKey{
    {GLFW_KEY_W, [](GLFWwindow* window){}},
    {GLFW_KEY_A, [](GLFWwindow* window){}},
    {GLFW_KEY_S, [](GLFWwindow* window){}},
    {GLFW_KEY_D, [](GLFWwindow* window){}},
    {GLFW_KEY_ESCAPE, [](GLFWwindow* window){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        }
    }
}
{}



