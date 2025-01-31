#ifndef DISCRETE_ENTITYMOVERIGHT_H
#define DISCRETE_ENTITYMOVERIGHT_H


#include "engine/Action.h"

class EntityMoveRight: public Action<> {
public:
    void operator()(std::shared_ptr<World> world) const override {}
};

#endif //DISCRETE_ENTITYMOVERIGHT_H
