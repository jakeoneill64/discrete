//
// Created by Jake M O'Neill on 24/09/2022.
//

#ifndef MRONEILLSCUBES_INPUT_H
#define MRONEILLSCUBES_INPUT_H

#include <unordered_map>
#include "GLFW/glfw3.h"

namespace discrete{

    class InputManager{
    public:
        //TODO use smart pointers
        static InputManager* activeInputManager;
        virtual void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
        virtual void onMouseEvent(GLFWwindow* window, double xpos, double ypos) = 0;
        virtual void onScrollEvent(){};
        virtual void onActivate(GLFWwindow* window){};
        virtual ~InputManager() = default;
    };


    class EntityInputManager: public InputManager{
    public:
        void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
        void onMouseEvent(GLFWwindow* window, double xpos, double ypos) override;
        void onActivate(GLFWwindow* window) override;
        EntityInputManager() noexcept;
    private:
        std::unordered_map<unsigned int, void (*)(GLFWwindow* window)> actionByKey;
    };


    void updateInputManager(GLFWwindow* window, InputManager* inputManager);


}



#endif //MRONEILLSCUBES_INPUT_H
