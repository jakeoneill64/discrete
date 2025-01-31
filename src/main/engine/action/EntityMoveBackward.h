#ifndef DISCRETE_ENTITYMOVEBACKWARD_H
#define DISCRETE_ENTITYMOVEBACKWARD_H

#include "engine/Action.h"

class EntityMoveBackward: public Action<> {
public:
    void operator()(std::shared_ptr<World> world) const override {}
};

#endif //DISCRETE_ENTITYMOVEBACKWARD_H
