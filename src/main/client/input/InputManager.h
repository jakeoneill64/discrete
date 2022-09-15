//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef MRONEILLSCUBES_INPUTMANAGER_H
#define MRONEILLSCUBES_INPUTMANAGER_H

#include "InputConfiguration.h"
#include "GLFW/glfw3.h"

namespace discrete::input{

    static InputConfiguration* inputConfiguration;
    void setInputConfiguration(InputConfiguration* inputConfiguration);
    void setInputContext();
    void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);


}

#endif //MRONEILLSCUBES_INPUTMANAGER_H
