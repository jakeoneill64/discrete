#ifndef MRONEILLSCUBES_INPUT_H
#define MRONEILLSCUBES_INPUT_H

#include <unordered_map>
#include "GLFW/glfw3.h"

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


#endif //MRONEILLSCUBES_INPUT_H
