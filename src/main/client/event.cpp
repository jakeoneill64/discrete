//
// Created by Jake M O'Neill on 26/09/2022.
//

#include "event.h"

using namespace discrete;

template<typename T>
void Event::subscribe(std::function<void(T)> callback) {
    std::lock_guard lock{s_mutex};
    s_eventManagersByTypes[typeid(T)].push_back(callback);
}

template<typename T>
void Event::publish(const T &event) {
    std::lock_guard lock{s_mutex};
    std::vector<std::function<void(T)>> subscribers = std::any_cast<std::vector<std::function<void(T)>>>(s_eventManagersByTypes[typeid(T)]);
    for(const auto& subscriber: subscribers){
        s_threadPool.submit([subscriber]{subscriber();});
    }
}