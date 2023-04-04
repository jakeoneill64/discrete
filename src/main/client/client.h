//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef DISCRETE_CLIENT_H
#define DISCRETE_CLIENT_H

#include <atomic>

namespace client{
    std::atomic<bool> should_run;
    int run();
}


#endif //DISCRETE_CLIENT_H
