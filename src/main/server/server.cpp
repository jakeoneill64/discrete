//
// Created by Jake M O'Neill on 05/11/2023.
//

#include "server.h"
#include "space.h"
#include <memory>


int server::run() {
    while(should_run){
        auto space = std::shared_ptr<Space>(new Space());

    }
    return 0;
}

