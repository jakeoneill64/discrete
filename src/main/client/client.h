//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef DISCRETE_CLIENT_H
#define DISCRETE_CLIENT_H

#include <atomic>
#include <memory>
#include "boost/url.hpp"
#include "GLFW/glfw3.h"
#include "engine/Engine.h"

// requirements
// client needs to send events via network call
// if the client is being run locally, the server and client will share a copy of the world,
// otherwise

struct ClientConfig{
    int GL_VERSION_MAJOR;
    int GL_VERSION_MINOR;
    int WINDOW_START_HEIGHT;
    int WINDOW_START_WIDTH;
    const char* WINDOW_TITLE;
};

struct DestroyGLFWWindow{

    void operator()(GLFWwindow* ptr);

};

//client -> window, render, audio, networking, input
//engine -> physics & logic, configurable as a game
//server -> networking, sync
class Client{
    public:
        // unfortunately this pattern (or similar workarounds) is necessary
        // for the glfw callbacks.
        static Client& instance(){
            static Client singleton{};
            return singleton;
        }
        void updateConfiguration(ClientConfig configuration);
    private:

        Client();


        //pass a callback to things that need to set this.
        std::atomic_bool m_shouldRun;
        std::unique_ptr<GLFWwindow, DestroyGLFWWindow> m_window;
        ClientConfig m_clientConfig;
        Engine m_engine;

        // the perspective that the client is currently bound to.
        u_int32_t m_entityId;

};


#endif //DISCRETE_CLIENT_H
