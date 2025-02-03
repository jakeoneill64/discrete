#ifndef DISCRETE_INPUTREPOSITORY_H
#define DISCRETE_INPUTREPOSITORY_H

#include "event/EventManager.h"
#include "persistence/sqlite3.h"
#include "engine/Action.h"
#include "input.h"
#include "Factory.h"
#include "util/types.h"
#include <shared_mutex>
#include <memory>
#include <unordered_map>

struct ActionKey {
    std::string inputKey;
    std::string context;

    bool operator==(const ActionKey& other) const noexcept {
        return inputKey == other.inputKey && context == other.context;
    }
};

template<>
struct std::hash<ActionKey> {

    std::size_t operator()(const ActionKey& key) const noexcept {
        std::size_t hashedActionKey = std::hash<std::string>{}(key.inputKey);
        hash_combine(hashedActionKey, key.context);
        return hashedActionKey;
    }

};

class InputRepository{
public:
    InputRepository(std::shared_ptr<EventManager> eventManager, std::shared_ptr<sqlite3> database);
    // NB an input 'key' may include something like MOUSE_CLICK_LEFT, KEY_F, KEY_ARROW_UP
    std::unique_ptr<Action<>> operator[](const std::string& inputKey, const std::string& context);
    ~InputRepository();
private:

    std::shared_ptr<EventManager> m_eventManager;
    std::shared_ptr<sqlite3> m_database;
    Factory<Action<>> m_actionFactory;
    std::unordered_map<ActionKey, std::string> m_inputMappings;
    std::unordered_map<sqlite3_int64, ActionKey> m_inputKeysById;
    std::shared_mutex m_mutex;
};

#endif //DISCRETE_INPUTREPOSITORY_H