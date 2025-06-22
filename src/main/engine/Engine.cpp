#include "Engine.h"

std::string Engine::getInputContext(uint32_t entityId) {
    return "default";
}

std::unique_ptr<WorldView> Engine::observeWorld() {
    return std::unique_ptr<WorldView>();
}

template<typename ActionType>
void Engine::submit(std::unique_ptr<Action<ActionType>> action) {
    action->operator();
}

Engine::Engine(const std::function<void()>& exitCallback): m_exitCallback{exitCallback}{}

