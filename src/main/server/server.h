//
// Created by Jake M O'Neill on 04/04/2023.
//

#ifndef MRONEILLSCUBES_SERVER_H
#define MRONEILLSCUBES_SERVER_H

#include <atomic>

namespace server{
    std::atomic<bool> should_run;
    int run(){}
}

#endif //MRONEILLSCUBES_SERVER_H
