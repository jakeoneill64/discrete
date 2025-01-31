#include "Engine.h"

std::string Engine::getInputContext(uint32_t entityId) {
    return "default";
}

template<typename ActionType>
void Engine::submit(std::unique_ptr<Action<ActionType>> action) {
    action->operator();
}
