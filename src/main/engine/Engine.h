#ifndef DISCRETE_ENGINE_H
#define DISCRETE_ENGINE_H

#include "event/EventManager.h"
#include "world.h"
#include "entity.h"
#include "Action.h"

#include <unordered_map>
#include <string>
#include <memory>

class Engine{

public:

    template <typename ActionType>
    void submit(std::unique_ptr<Action<ActionType>> action);
    std::string getInputContext(uint32_t entityId);
    std::unique_ptr<WorldView> observeWorld();

private:

    std::shared_ptr<World> m_world;

};


#endif //DISCRETE_ENGINE_H


// EVENT           -> Statemachine, Runtime Polymorphism {Actions, Game Logic is here} ->
// mousePosEvent   -> entityCamPosStatemachine { entityCamPosStateMachine (update entity in world } ->
// frameStartEvent ->