#ifndef DISCRETE_ENTITYMOVELEFT_H
#define DISCRETE_ENTITYMOVELEFT_H

#include "engine/Action.h"

class EntityMoveLeft: public Action<> {
public:
    void operator()(std::shared_ptr<World> world) const override {}
};

#endif //DISCRETE_ENTITYMOVELEFT_H
