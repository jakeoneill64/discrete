//
// Created by Jake M O'Neill on 26/09/2022.
//

#ifndef DISCRETE_EVENT_H
#define DISCRETE_EVENT_H

#include <functional>

namespace event{

    template<typename T>
    static void subscribe(const std::type_info& type_info, std::function<void(T)> callback);

    template<typename T>
    void publish(const T& event);

}


#endif DISCRETE_EVENT_H
