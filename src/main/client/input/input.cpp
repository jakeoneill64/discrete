//
// Created by Jake M O'Neill on 24/09/2022.
//

#include "input.h"

using namespace discrete;


void EntityInputManager::onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto function = actionByKey.find(key);
    if(function != actionByKey.end()) function->second(window);
}

void EntityInputManager::onMouseEvent() {

}

void EntityInputManager::onActivate(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, [](){onKeyEvent})
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
{

}

