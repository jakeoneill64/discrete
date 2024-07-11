//
// Created by Jake M O'Neill on 05/11/2023.
//

#include "server.h"

Server& Server::run(u_int32_t port) {
//    auto space = std::shared_ptr<Space>(new Space());
//    run(space, port);
}

//Server& Server::run(std::shared_ptr<Space> space, u_int32_t port) {
//    Server* server{new Server()};
//    server->loop(space);
//    return *server;
//}

void Server::stop() {
    // TODO unbind
    should_run = false;
}

//void Server::loop(std::shared_ptr<Space> space) {
//    while(should_run){
//        // Receive events
//        // step physics
//    }
//}
