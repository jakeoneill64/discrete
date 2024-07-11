#ifndef DISCRETE_INPUT_H
#define DISCRETE_INPUT_H

#include <unordered_map>
#include "GLFW/glfw3.h"
#include <filesystem>

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

// define every action that may be performed as an input here. These will
// be mapped to individual keys in the default / modified keymapping file.
// these actions will be mapped to calls to the engine.
enum InputAction{
    ENTITY_MOVE_FORWARD,
    ENTITY_MOVE_BACKWARD,
    ENTITY_MOVE_LEFT,
    ENTITY_MOVE_RIGHT
};

std::unordered_map<char, InputAction> load_default_input_mappings();
std::unordered_map<char, InputAction> load_input_mappings(std::filesystem::path mappingFile);

#endif //DISCRETE_INPUT_H
