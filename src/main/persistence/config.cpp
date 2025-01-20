#include "config.h"
#include "sqlite3.h"

std::string ConfigDao::get(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock{m_mutex};
    return m_configMap.at(key);
}

void ConfigDao::set(const std::string &key, const std::string &value) {
    std::unique_lock<std::shared_mutex> lock{m_mutex};
}
