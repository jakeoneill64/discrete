#ifndef DISCRETE_CLIENT_H
#define DISCRETE_CLIENT_H
#define GLFW_INCLUDE_VULKAN

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "sqlite3.h"
#include "engine/Engine.h"

#include <atomic>
#include <memory>

struct DestroyGLFWWindow{

    void operator()(GLFWwindow* ptr) const;

};

//client -> window, render, audio, networking, input
//engine -> physics & logic, configurable as a game
//server -> networking, sync
class Client {

    public:
        // unfortunately this pattern (or similar workarounds) is necessary
        // for the glfw callbacks.
        static Client& instance(){
            static Client singleton{};
            return singleton;
        }

    private:
        Client();
        void loop();

        //pass a callback to things that need to set this.
        std::atomic_bool m_shouldRun;
        std::unique_ptr<GLFWwindow, DestroyGLFWWindow> m_window;
        uint32_t m_boundEntityId{};
        std::shared_ptr<EventManager> m_eventManager;
        std::shared_ptr<sqlite3> m_database;
        VkInstance m_vulkanInstance{};
        VkDevice m_device{};
        Engine m_engine;

};


#endif //DISCRETE_CLIENT_H
