#ifndef DISCRETE_INPUTREPOSITORY_H
#define DISCRETE_INPUTREPOSITORY_H

#include "event/EventManager.h"
#include "sqlite3.h"
#include "engine/Action.h"
#include "input.h"
#include "Factory.h"
#include <memory>
#include <unordered_map>

class InputRepository{
public:
    InputRepository(std::shared_ptr<EventManager> eventManager, std::shared_ptr<sqlite3> database);
    // NB an input 'key' may include something like MOUSE_CLICK_LEFT, KEY_F, KEY_ARROW_UP
    std::unique_ptr<Action<>> operator[](const std::string& inputKey);
private:
    std::shared_ptr<EventManager> m_eventManager;
    std::shared_ptr<sqlite3> m_database;
    Factory<Action<>> m_actionFactory;
    std::unordered_map<std::string, std::string> m_inputMappings;
    std::unordered_map<sqlite3_int64, std::string> m_inputKeyById;
};

#endif //DISCRETE_INPUTREPOSITORY_H