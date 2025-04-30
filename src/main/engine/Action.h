#ifndef DISCRETE_ACTION_H
#define DISCRETE_ACTION_H

#include <memory>
#include <functional>
#include <optional>
#include "engine/world.h"

namespace {
    template<typename T>
    struct ActionHelper {

        struct Type {
            explicit Type(T t);
            virtual void operator()(std::shared_ptr<World> world) const = 0;
            virtual ~Type() = default;
        private:
            T data;
        };
    };

    template<>
    struct ActionHelper<void> {
        struct Type {
            Type() = default;
            virtual void operator()(std::shared_ptr<World> world) const = 0;
            virtual ~Type() = default;
        };
    };
}


/***
 *  Entities interact with the engine and world via actions. Some actions are stateless (default - Action<void>)
 *  and some are created with state.
 */
template <typename T = void>
using Action = typename ActionHelper<T>::Type;

#endif //DISCRETE_ACTION_H