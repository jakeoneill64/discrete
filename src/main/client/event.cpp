//
// Created by Jake M O'Neill on 26/09/2022.
//

#include "event.h"

using namespace discrete;

template <typename T>
void EventManager<T>::subscribe(std::function<void(T)> callback){
    std::lock_guard lock{m_mutex};
    m_subscribers.push_back(callback);
}

template <typename T>
void EventManager<T>::publish(const T&){
    std::lock_guard lock{m_mutex};
    for(const auto& subscriber: m_subscribers)
        m_threadPool(subscriber());
}