#include "sqlite3.h"
#include "database.h"
#include "ConfigRepository.h"

#include <optional>
#include <string>
#include <tuple>
#include <format>

ConfigRepository::ConfigRepository(
std::shared_ptr<EventManager> databaseEventManager, std::shared_ptr<sqlite3> database) noexcept
:
    m_eventManager{std::move(databaseEventManager)},
    m_database{std::move(database)},
    m_configMap{},
    m_keysByIds{}
{
    std::ranges::for_each(
        getAllRecords<sqlite3_int64 , std::string, std::string>(m_database.get(), "config"),
        [&configMap = m_configMap, &keysById = m_keysByIds]
        (const auto& record){
            const auto& [id, key, value] = record;
                configMap[key] = value;
                keysById[id] = key;
        }
    );


    m_eventManager->subscribeEvent<ConfigUpdate>(
        [&]
        (ConfigUpdate updateEvent){
            std::unique_lock<std::shared_mutex> writeLock{m_mutex};
            const auto& [operation, rowId] = updateEvent;
            switch (operation) {
                case SQLITE_DELETE:
                    m_configMap.erase(m_keysByIds[rowId]);
                    m_keysByIds.erase(rowId);
                    break;
                case SQLITE_INSERT:
                case SQLITE_UPDATE: {
                        const auto & record =
                            getRecord<sqlite3_int64,std::string,std::string>(m_database.get(), "config", rowId);
                        const auto &[_1, key, value] = record.value();
                            m_configMap[key] = value;
                            m_keysByIds[rowId] = key;
                    }
                    break;
                default:
                    break;
            }
        }
    );
}

ConfigRepository::~ConfigRepository() {
    m_unsubscribeConfigEvents();
}

std::optional<std::string> ConfigRepository::operator[](const std::string &key) {
    std::shared_lock<std::shared_mutex> lock{m_mutex};
    return m_configMap.contains(key) ? std::make_optional(m_configMap[key]) : std::nullopt;
}

void ConfigRepository::insert(const std::string &key, const std::string &value) {

    std::unique_lock<std::shared_mutex> lock{m_mutex};

    const auto& record =
        getRecord<std::string, sqlite3_int64, std::string, std::string>(m_database.get(), "config", "key", key);

    std::string statementString = record.transform(
        [&key = key, &value = value](const auto& existingRecord){
            const auto& [id, existingKey, existingValue] = existingRecord;
            return std::format("UPDATE config SET key={}, value={} WHERE id={};", key, value, id);
        }
    ).value_or(
        std::format("INSERT INTO config (key, value) VALUES ({}, {});", key, value)
    );

    sqlite3_stmt* statement;
    // TODO handle failures. Don't want to add a bunch of noise here so will try to
    // macro this or use some other mechanism to keep it clean.
    sqlite3_prepare_v2(m_database.get(), statementString.c_str(), -1, &statement, nullptr);
    sqlite3_step(statement);
    sqlite3_finalize(statement);

}