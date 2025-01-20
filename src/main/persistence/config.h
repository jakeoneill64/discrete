#include <shared_mutex>
#include <unordered_map>
#include <string>

class ConfigDao {
public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
private:
    std::unordered_map<std::string, std::string> m_configMap;
    std::shared_mutex m_mutex;

};