#ifndef DISCRETE_INPUT_H
#define DISCRETE_INPUT_H

#include <unordered_map>
#include "glfw3.h"

const std::unordered_map<int, std::string> GLFW_KEY_MAPPINGS {
    {GLFW_KEY_W, "KEY_W"},
    {GLFW_KEY_A, "KEY_A"},
    {GLFW_KEY_S, "KEY_S"},
    {GLFW_KEY_D, "KEY_D"},
};

const std::unordered_map<int, std::string> GLFW_MOUSE_BUTTON_MAPPINGS {
    {GLFW_MOUSE_BUTTON_LEFT, "MOUSE_CLICK_LEFT"},
    {GLFW_MOUSE_BUTTON_LEFT, "MOUSE_CLICK_RIGHT"},
};

struct InputPressEvent{
    int key;
    int action;
    int mods;
};

using KeyEvent = InputPressEvent;
using MouseButtonEvent = InputPressEvent;

struct MousePositionEvent {
    double xPos;
    double yPos;
};

#endif //DISCRETE_INPUT_H
