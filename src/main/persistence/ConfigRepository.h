#ifndef DISCRETE_CONFIG_H
#define DISCRETE_CONFIG_H

#include "event/EventManager.h"
#include "persistence/sqlite3.h"
#include <shared_mutex>
#include <unordered_map>
#include <string>
#include <functional>
#include <optional>
#include <memory>

// TODO very common pattern, might make sense to make this a bit more generic.
class ConfigRepository{
public:
    ConfigRepository(
        std::shared_ptr<EventManager> databaseEventManager,
        std::shared_ptr<sqlite3> database
    ) noexcept;
    ~ConfigRepository();
    std::optional<std::string> operator[](const std::string &key);
    virtual void insert(const std::string& key, const std::string& value);
private:
    std::shared_mutex m_mutex;
    std::shared_ptr<EventManager> m_eventManager;
    std::shared_ptr<sqlite3> m_database;
    std::unordered_map<std::string, std::string> m_configMap;
    std::unordered_map<sqlite3_int64, std::string> m_keysByIds;
    std::function<void()> m_unsubscribeConfigEvents;
};

#endif //DISCRETE_CONFIG_H