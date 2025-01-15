#ifndef DISCRETE_MOVEENTITY_H
#define DISCRETE_MOVEENTITY_H

#include "engine/Action.h"

class EntityMove: public Action{
    explicit EntityMove(const vec3& direction, unsigned int entityId);
    void operator()(std::shared_ptr<World> world) const override;
};


#endif //DISCRETE_MOVEENTITY_H
