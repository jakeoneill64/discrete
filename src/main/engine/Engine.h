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

    // Client / Server           Engine
    // (NETWORK)             <-> (SYNC, EVENTS)                    Audio
    // (CONTEXT, GLFW EVENT)  -> (ACTION / STATEMACHINE, WORLD) -> Render Pipeline

private:

    World m_world;
    std::vector<Entity> m_entities;

};


#endif //DISCRETE_ENGINE_H
