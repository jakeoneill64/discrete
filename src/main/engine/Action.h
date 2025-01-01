#ifndef DISCRETE_ACTION_H
#define DISCRETE_ACTION_H

#include <memory>
#include <functional>
#include <optional>

class Action {
public:
    virtual void operator()() const = 0;
    virtual ~Action() = default;
    const std::optional<std::function<std::unique_ptr<Action>()>> onComplete;
    const std::function<std::unique_ptr<Action>()> onError;
};

#endif //DISCRETE_ACTION_H