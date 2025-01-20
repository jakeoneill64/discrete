#ifndef DISCRETE_ACTION_H
#define DISCRETE_ACTION_H

#include "world.h"
#include <memory>
#include <functional>
#include <optional>

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

template <typename T = void>
using Action = typename ActionHelper<T>::Type;

#endif //DISCRETE_ACTION_H