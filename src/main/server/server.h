//
// Created by Jake M O'Neill on 04/04/2023.
//

#ifndef MRONEILLSCUBES_SERVER_H
#define MRONEILLSCUBES_SERVER_H

#include <atomic>
#include "space.h"


// server and client should be able to share physics and events
// server receives and sends events over the network in UDP packets
// with a synchronisation event less frequently
class Server{
    public:
        static Server& run(uint32_t binding);
        static Server& run(std::shared_ptr<Space> space, u_int32_t binding);
        void stop();
    private:
        Server() = default;
        void loop(std::shared_ptr<Space> space);
        std::atomic<bool> should_run;
};

#endif //MRONEILLSCUBES_SERVER_H
