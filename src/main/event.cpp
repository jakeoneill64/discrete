//
// Created by Jake M O'Neill on 26/09/2022.
//

#include "event.h"
#include "threadpool.h"

#include <unordered_map>
#include <any>

static std::mutex s_mutex;

//TODO all calls are unreachable?
static std::unordered_map<std::type_info, std::vector<std::function<void(std::any)>>>& get_event_managers_by_type_info(){
    static std::unordered_map<std::type_info, std::vector<std::function<void(std::any)>>> event_managers_by_type_info;
    return event_managers_by_type_info;
};

template<typename T>
void event::subscribe(const std::type_info& event_type, std::function<void(T)> callback) {
    std::lock_guard lock{s_mutex};
    get_event_managers_by_type_info()[typeid(T)].push_back(callback);
}

template<typename T>
void event::publish(const T &event) {
    std::lock_guard lock{s_mutex};

    auto subscribers{
            std::any_cast<std::vector<std::function<void(T)>>>(get_event_managers_by_type_info()[typeid(T)])
    };

    for(const auto& subscriber: subscribers){
        shared_threadpool().submit([subscriber]{subscriber();});
    }
}