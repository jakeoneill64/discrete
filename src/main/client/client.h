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
class Client{
    public:
        static Client& run(const boost::url& serverURL);
        void stop();
    private:
        Client() = default;
        void loop();
        std::atomic<bool> should_run;
};


#endif //DISCRETE_CLIENT_H
