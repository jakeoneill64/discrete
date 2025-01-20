#ifndef DISCRETE_ENGINE_H
#define DISCRETE_ENGINE_H

#include "event/EventManager.h"
#include "world.h"
#include "entity.h"
#include "util/vector.h"
#include "Action.h"

#include <unordered_map>
#include <string>

// encapsulates spacial / world data
class Engine{

public:

    // Client / Server           Engine
    // (NETWORK)             <-> (SYNC, EVENTS)                    Audio
    // (CONTEXT, GLFW EVENT)  -> (ACTION / STATEMACHINE, WORLD) -> Render Pipeline

//    void operator()(const std::string& stateMachine, const Action& initialState);

private:

    World m_world;
//    std::unordered_map<std::string, Action> actionsByName;
    std::string getInputContext(uint32_t entityId);

};


#endif //DISCRETE_ENGINE_H


// EVENT           -> Statemachine, Runtime Polymorphism {Actions, Game Logic is here} ->
// mousePosEvent   -> entityCamPosStatemachine { entityCamPosStateMachine (update entity in world } ->
// frameStartEvent ->