#ifndef DISCRETE_INPUT_H
#define DISCRETE_INPUT_H

#include <unordered_map>
#include "GLFW/glfw3.h"

const std::unordered_map<int, char> GLFW_KEY_MAPPINGS{
    {GLFW_KEY_W, 'w'},
    {GLFW_KEY_A, 'a'},
    {GLFW_KEY_S, 's'},
    {GLFW_KEY_D, 'd'},
};

struct KeyEvent{
    int key;
    int scancode;
    int action;
    int mods;
};

struct MouseEvent{
    double xPos;
    double yPos;
};

#endif //DISCRETE_INPUT_H
