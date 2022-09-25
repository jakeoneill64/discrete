//
// Created by Jake M O'Neill on 24/09/2022.
//

#ifndef MRONEILLSCUBES_INPUT_H
#define MRONEILLSCUBES_INPUT_H

#include <unordered_map>
#include "GLFW/glfw3.h"

namespace discrete{

    enum InputContext{WORLD_ENTITY};

    class InputManager{
    public:
        virtual void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
        virtual void onMouseEvent() = 0;
        virtual void onScrollEvent(){};
        virtual void onActivate(GLFWwindow* window) = 0;
    };

    class EntityInputManager: public InputManager{
    public:
        void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
        void onMouseEvent() override;
        void onActivate(GLFWwindow* window) override;
        EntityInputManager() noexcept;
    private:
        std::unordered_map<unsigned int, void (*)(GLFWwindow* window)> actionByKey;
    };

    void addInputManager(){
        //TODO implement. if already present in the managerByContext map, destroy first!
    }

    static std::unordered_map<InputContext, InputManager*> managerByContext{
            {WORLD_ENTITY, new EntityInputManager{}}
    };


}



#endif //MRONEILLSCUBES_INPUT_H
