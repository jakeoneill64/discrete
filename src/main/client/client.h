//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef DISCRETE_CLIENT_H
#define DISCRETE_CLIENT_H

#include <atomic>
#include "boost/url.hpp"

// requirements
// client needs to send events via network call
// if the client is being run locally, the server and client will share a copy of the world,
// otherwise

struct ClientConfig{
    uint GL_VERSION_MAJOR;
    uint GL_VERSION_MINOR;
    uint WINDOW_START_HEIGHT;
    uint WINDOW_START_WIDTH;
    const char* WINDOW_TITLE;
};

//client -> window, render, audio, networking, input
//engine -> physics & logic, configurable as a game
//server -> networking, sync
class Client{
    public:
        Client();
        void updateConfiguration(ClientConfig configuration);
        void stop();
    private:
        void start();
        std::atomic<bool> m_shouldRun;
        ClientConfig m_clientConfig;
};


#endif //DISCRETE_CLIENT_H
