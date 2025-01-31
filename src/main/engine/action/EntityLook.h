#ifndef DISCRETE_ENTITYLOOK_H
#define DISCRETE_ENTITYLOOK_H

#include "../Action.h"
#include "util/vector.h"

struct EntityLookParam {
    uint32_t entityId;
    double x;
    double y;
};

class EntityLook: public Action<EntityLookParam>{
    using Action<EntityLookParam>::Action;
    inline void operator()(std::shared_ptr<World> world) const override{}
};

#endif //DISCRETE_ENTITYLOOK_H
