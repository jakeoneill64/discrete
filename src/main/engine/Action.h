#ifndef DISCRETE_ACTION_H
#define DISCRETE_ACTION_H

#include "world.h"
#include <memory>
#include <functional>
#include <optional>

class Action {
public:
    virtual void operator()(std::shared_ptr<World> world) const = 0;
    virtual ~Action() = default;
};

#endif //DISCRETE_ACTION_H