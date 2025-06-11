#ifndef DISCRETE_EVENTMANAGER_H
#define DISCRETE_EVENTMANAGER_H

#include <any>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <shared_mutex>
#include "threadpool.h"

class EventManager{

    public:

        template<typename EventType>
        void publishEvent(const EventType& event){
            std::shared_lock lock{m_mutex};
            m_callbacksByEventType.contains(std::type_index(typeid(EventType)));
            for(const auto& [_, callback]: m_callbacksByEventType[std::type_index(typeid(EventType))]){
                m_threadPool.submit([event, callback](){callback(event);});
            }
        }

        /**
         *
         * @tparam EventType
         * @param callback
         * @return unsubscribe function
         */
        template <typename EventType>
        std::function<void()>
        subscribeEvent(const std::function<void(const EventType&)>& callback){

            std::unique_lock lock{m_mutex};

            auto sequenceNumber{m_callbackIdSequence};
            m_callbackIdSequence++;

            const auto& wrappedCallback = [callback](std::any value){
                callback(std::any_cast<const EventType&>(value));
            };

            if(!m_callbacksByEventType.contains(std::type_index(typeid(EventType))))
                m_callbacksByEventType.emplace(std::type_index(typeid(EventType)), std::unordered_map<uint32_t, std::function<void(std::any)>>());

            m_callbacksByEventType[std::type_index(typeid(EventType))][sequenceNumber] = wrappedCallback;
            return [this, sequenceNumber](){m_callbacksByEventType[typeid(EventType)].erase(sequenceNumber);};
        }

        EventManager(): m_callbackIdSequence{0}{}

    private:

        std::unordered_map<
            std::type_index,
            std::unordered_map<
                u_int32_t,
                std::function<void(std::any)>
            >
        > m_callbacksByEventType;

        // don't need atomic because it is already within control block in
        // std::function<void()> subscribe();
        std::uint32_t m_callbackIdSequence;
        std::shared_mutex m_mutex;
        ThreadPool m_threadPool;

};


#endif //DISCRETE_EVENTMANAGER_H
