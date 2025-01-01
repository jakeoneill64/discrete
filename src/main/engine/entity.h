#include "Action.h"
#include "util/vector.h"

#include <cinttypes>

class EntityMoveAction: public Action{
    explicit EntityMoveAction(const vec3& direction);
    void operator()() const override;
};

class Entity{
public:
    const u_int32_t id;
};

