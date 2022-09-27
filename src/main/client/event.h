//
// Created by Jake M O'Neill on 26/09/2022.
//

#ifndef MRONEILLSCUBES_EVENT_H
#define MRONEILLSCUBES_EVENT_H

#include "threadpool.h"

namespace discrete {

    template<typename T>
    class EventManager {
    public:
        void subscribe(std::function<void(T)> callback);
        void publish(const T&);
    private:
        std::vector<std::function<void(T)>> m_subscribers;
        std::mutex m_mutex;
        ThreadPool m_threadPool;
    };


}


#endif //MRONEILLSCUBES_EVENT_H
