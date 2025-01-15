#ifndef DISCRETE_CLIENT_H
#define DISCRETE_CLIENT_H

#include <atomic>
#include <memory>
#include "GLFW/glfw3.h"
#include "engine/Engine.h"

// requirements
// client needs to send events via network call
// if the client is being run locally, the server and client will share a copy of the world,
// otherwise

struct DestroyGLFWWindow{

    void operator()(GLFWwindow* ptr) const;

};

//client -> window, render, audio, networking, input
//engine -> physics & logic, configurable as a game
//server -> networking, sync
class Client : public EventManager{
    public:


    private:

        // unfortunately this pattern (or similar workarounds) is necessary
        // for the glfw callbacks.
        static Client& instance(){
            static Client singleton{};
            return singleton;
        }
        Client();

        //pass a callback to things that need to set this.
        std::atomic_bool m_shouldRun;
        std::unique_ptr<GLFWwindow, DestroyGLFWWindow> m_window;
        Engine m_engine;

};


#endif //DISCRETE_CLIENT_H
