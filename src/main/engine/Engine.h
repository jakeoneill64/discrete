#ifndef DISCRETE_ENGINE_H
#define DISCRETE_ENGINE_H

#include "event/EventManager.h"
#include "world.h"
#include "entity.h"
#include <vector>
#include "util/vector.h"

// encapsulates space / world info
// handles events
// steps physics / game logic.
class Engine : public EventManager{

public:
    void moveEntity(const int& entityId, const vec3& displacement);
private:

    //TODO TBD datastructure determination.
    World m_world;
    std::vector<Entity> m_entities;

};


#endif //DISCRETE_ENGINE_H
