//
// Created by Jake M O'Neill on 04/04/2023.
//

#include "server/server.h"
#include "client/client.h"
#include <future>

int main(){

    auto server_result = std::async(server::run);
    auto client_result = std::async(client::run);

    return server_result.get() & client_result.get();

}