#ifndef DISCRETE_MOVEENTITY_H
#define DISCRETE_MOVEENTITY_H

#include "engine/Action.h"
#include "util/vector.h"
#include <memory>

class EntityMove: public Action<vec3>{
    using Action<vec3>::Action;
    void operator()(std::shared_ptr<World> world) const override;
};

class EntityMoveForward : public EntityMove {
public:
    EntityMoveForward();
};

class EntityMoveBackward : public EntityMove {
public:
    EntityMoveBackward();
};

class EntityMoveLeft : public EntityMove {
public:
    EntityMoveLeft();
};


class EntityMoveRight : public EntityMove {
public:
    EntityMoveRight();
};


#endif //DISCRETE_MOVEENTITY_H