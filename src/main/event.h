//
// Created by Jake M O'Neill on 26/09/2022.
//

#ifndef DISCRETE_EVENT_H
#define DISCRETE_EVENT_H

#include <functional>
#include <string>
#include <memory>

namespace event{

    void subscribe(const char *, std::function<void()> callback);
    void subscribe(const std::string&, std::function<void()> callback);

    // TODO for the data, we probably want std::shared_ptr of some type
    void publish(const char * event);
    void publish(const std::string &event);

}


#endif //DISCRETE_EVENT_H