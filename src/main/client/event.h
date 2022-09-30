//
// Created by Jake M O'Neill on 26/09/2022.
//

#ifndef MRONEILLSCUBES_EVENT_H
#define MRONEILLSCUBES_EVENT_H

#include "threadpool.h"
#include <unordered_map>
#include <any>


namespace discrete {

    class Event{
    public:
        template<typename T>
        static void subscribe(std::function<void(T)> callback);

        template<typename T>
        static void publish(const T& event);
    private:
        static std::unordered_map<std::type_info, std::vector<std::function<void(std::any)>>> s_eventManagersByTypes;
        static std::mutex s_mutex;
        static ThreadPool s_threadPool;
    };

}


#endif //MRONEILLSCUBES_EVENT_H
