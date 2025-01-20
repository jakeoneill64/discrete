#ifndef DISCRETE_ENTITYLOOK_H
#define DISCRETE_ENTITYLOOK_H

#include "../Action.h"
#include "util/vector.h"

class EntityLook: public Action<vec2>{
    explicit EntityLook(const vec2& direction, unsigned int entityId);
    void operator()(std::shared_ptr<World> world) const override;
};

#endif //DISCRETE_ENTITYLOOK_H
