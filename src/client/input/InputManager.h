//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef MRONEILLSCUBES_INPUTMANAGER_H
#define MRONEILLSCUBES_INPUTMANAGER_H

#include "InputConfiguration.h"
#include <GLFW/glfw3.h>

class InputManager{
public:

    InputManager(const InputConfiguration& configuration, const GLFWwindow* window);
    void setInputConfiguration(const InputConfiguration& configuration);

private:

    void onKeyboardEvent(const GLFWwindow* window, int key, int scancode, int action, int mods);
    InputConfiguration m_configuration;

};

#endif //MRONEILLSCUBES_INPUTMANAGER_H
