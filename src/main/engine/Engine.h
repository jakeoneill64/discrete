#ifndef DISCRETE_ENGINE_H
#define DISCRETE_ENGINE_H

#include "event/EventManager.h"
#include "world.h"
#include "entity.h"
#include "util/vector.h"
#include "Action.h"

#include <unordered_map>

// encapsulates spacial / world data
class Engine{

public:

    // Client / Server           Engine
    // (NETWORK)             <-> (SYNC, EVENTS)                    Audio
    // (CONTEXT, GLFW EVENT)  -> (ACTION / STATEMACHINE, WORLD) -> Render Pipeline
    

private:

    // load statemachines and index.
//    std::unordered_map<std::string, StateMachine> stateMachinesByName;
    World m_world;

};


#endif //DISCRETE_ENGINE_H


// EVENT           -> Statemachine, Runtime Polymorphism {Actions, Game Logic is here} ->
// mousePosEvent   -> entityCamPosStatemachine { entityCamPosStateMachine (update entity in world } ->
// frameStartEvent ->