#ifndef DISCRETE_CLIENT_H
#define DISCRETE_CLIENT_H

#include "GLFW/glfw3.h"
#include "engine/Engine.h"
#include "./input/input.h"
#include "persistence/sqlite3.h"
#include "persistence/ConfigRepository.h"
#include "Factory.h"
#include "engine/Action.h"
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
        Client();

    private:
        // unfortunately this pattern (or similar workarounds) is necessary
        // for the glfw callbacks.
        static inline Client& instance(){
            static Client singleton{};
            return singleton;
        }

        //pass a callback to things that need to set this.
        std::atomic_bool m_shouldRun;
        std::unique_ptr<GLFWwindow, DestroyGLFWWindow> m_window;
        uint32_t m_boundEntityId{};
        std::shared_ptr<EventManager> m_eventManager;
        std::shared_ptr<sqlite3> m_database;
        ConfigRepository m_configRepository;

        Engine m_engine;

};


#endif //DISCRETE_CLIENT_H
